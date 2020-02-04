package saxon.board.digilent

import saxon.{ResetSensitivity, _}
import spinal.core._
import spinal.core.sim._
import spinal.lib.blackbox.xilinx.s7.{BUFG, STARTUPE2}
import spinal.lib.bus.amba3.apb.Apb3Config
import spinal.lib.bus.amba3.apb.sim.{Apb3Listener, Apb3Monitor}
import spinal.lib.com.jtag.sim.JtagTcp
import spinal.lib.com.spi.SpiHalfDuplexMaster
import spinal.lib.com.spi.ddr.{SpiXdrMasterCtrl, SpiXdrParameter}
import spinal.lib.com.uart.UartCtrlMemoryMappedConfig
import spinal.lib.com.uart.sim.{UartDecoder, UartEncoder}
import spinal.lib.eda.bench.{Bench, Rtl, XilinxStdTargets}
import spinal.lib.generator._
import spinal.lib.io.{Gpio, InOutWrapper}
import spinal.lib.master
import spinal.lib.memory.sdram.sdr._
import spinal.lib.memory.sdram.sdr.sim.SdramModel
import spinal.lib.memory.sdram.xdr.CoreParameter
import spinal.lib.memory.sdram.xdr.phy.XilinxS7Phy
import vexriscv.plugin.CsrPlugin




class Arty7LinuxSystem() extends SaxonSocLinux{
  //Add components
  val gpioA = Apb3GpioGenerator(0x00000)
  val gpioB = new Apb3GpioGenerator(0x40000)/*{
      //val gpioB_phy = produce(logic.io.gpio)//does nothing
      //produce(out(Bool).setName("gpioB0") := logic.io.gpio(0).write)//ok but inout remains
  }*/
  val spiA = new Apb3SpiGenerator(0x20000){
    val user = produce(master(phy.withoutSs.toSpi()).setName("system_spiA_user")) //TODO automatic naming
    val flash = produce(master(phy.decode(ssId = 0).toSpi()).setName("system_spiA_flash")) //TODO automatic naming
    val sdcard = produce(master(phy.decode(ssId = 1).toSpi()).setName("system_spiA_sdcard")) //TODO automatic naming
  }

  val spiB = new Apb3SpiGenerator(0x30000, xipOffset = 0x30000000){
      //val user = produce(master(phy.withoutSs.toSpi()).setName("system_spiB_user"))
      produce(phy.data.foreach(_.read.assignDontCare()))
      val flash = produce(master(phy.decode(ssId = 0).toSpi()).setName("system_spiB_flash"))
  }

  val ramA = BmbOnChipRamGenerator(0xE0000000l)
  ramA.dataWidth.load(32)

  val sdramA = SdramXdrBmbGenerator(
    memoryAddress = 0x80000000l
  )

  val sdramA0 = sdramA.addPort()

  val bridge = BmbBridgeGenerator()
  interconnect.addConnection(
    cpu.iBus -> List(bridge.bmb),
    cpu.dBus -> List(bridge.bmb),
    bridge.bmb -> List(ramA.bmb, sdramA0.bmb, peripheralBridge.input)
  )

//Interconnect specification
//  interconnect.addConnection(
//    cpu.iBus -> List(sdramA0.bmb),
//    cpu.dBus -> List(sdramA0.bmb, peripheralBridge.input)
//  )


  //  val bridge = BmbBridgeGenerator()
//  interconnect.addConnection(
//    cpu.iBus -> List(sdramA1.bmb),
//    cpu.dBus -> List(ramA.bmb, sdramA0.bmb, peripheralBridge.input)
//  )
//
//  interconnect.setConnector(sdramA1.bmb){case (m,s) =>
//    m.cmd.halfPipe() >> s.cmd
//    m.rsp << s.rsp.halfPipe()
//  }
}

class Arty7Linux extends Generator{
  val mainClockCtrl = ClockDomainGenerator()
  mainClockCtrl.resetHoldDuration.load(255)
  mainClockCtrl.resetSynchronous.load(false)
  mainClockCtrl.powerOnReset.load(true)
  mainClockCtrl.resetBuffer.load(e => BUFG.on(e))


  val sdramClockCtrl = ClockDomainGenerator()
  sdramClockCtrl.resetHoldDuration.load(0)
  sdramClockCtrl.resetSynchronous.load(false)
  sdramClockCtrl.powerOnReset.load(false)
  sdramClockCtrl.resetBuffer.load(e => BUFG.on(e))
  sdramClockCtrl.resetSensitivity.load(ResetSensitivity.HIGH)
  mainClockCtrl.clockDomain.produce(sdramClockCtrl.reset.load(mainClockCtrl.clockDomain.reset))


  val system = new Arty7LinuxSystem()
  system.onClockDomain(mainClockCtrl.clockDomain)

  system.sdramA.onClockDomain(sdramClockCtrl.clockDomain)

  val sdramDomain = new Generator{
    onClockDomain(sdramClockCtrl.clockDomain)

    val apbDecoder = Apb3DecoderGenerator()
    apbDecoder.addSlave(system.sdramA.apb, 0x0000)

    val phyA = XilinxS7PhyGenerator(configAddress = 0x1000)(apbDecoder)
    phyA.connect(system.sdramA)

    val sdramApbBridge = Apb3CCGenerator() //TODO size optimisation
    sdramApbBridge.mapAt(0x100000l)(system.apbDecoder)
    sdramApbBridge.setOutput(apbDecoder.input)
    sdramApbBridge.inputClockDomain.merge(mainClockCtrl.clockDomain)
    sdramApbBridge.outputClockDomain.merge(sdramClockCtrl.clockDomain)
  }

  val clocking = add task new Area{
    val GCLK100 = in Bool()
    val cpu_reset = in Bool()

    mainClockCtrl.clkFrequency.load(100 MHz)
    sdramClockCtrl.clkFrequency.load(150 MHz)
    val pll = new BlackBox{
      setDefinitionName("PLLE2_ADV")

      addGenerics(
        "CLKIN1_PERIOD" -> 10.0,
        "CLKFBOUT_MULT" -> 12,
        "CLKOUT0_DIVIDE" -> 12,
        "CLKOUT0_PHASE" -> 0,
        "CLKOUT1_DIVIDE" -> 8,
        "CLKOUT1_PHASE" -> 0,
        "CLKOUT2_DIVIDE" -> 8,
        "CLKOUT2_PHASE" -> 45,
        "CLKOUT3_DIVIDE" -> 4,
        "CLKOUT3_PHASE" -> 0,
        "CLKOUT4_DIVIDE" -> 4,
        "CLKOUT4_PHASE" -> 90
      )

      val CLKIN1   = in Bool()
      val CLKFBIN  = in Bool()
      val CLKFBOUT = out Bool()
      val CLKOUT0  = out Bool()
      val CLKOUT1  = out Bool()
      val CLKOUT2  = out Bool()
      val CLKOUT3  = out Bool()
      val CLKOUT4  = out Bool()

      Clock.syncDrive(CLKIN1, CLKOUT1)
      Clock.syncDrive(CLKIN1, CLKOUT2)
      Clock.syncDrive(CLKIN1, CLKOUT3)
      Clock.syncDrive(CLKIN1, CLKOUT4)
    }

    pll.CLKFBIN := pll.CLKFBOUT
    pll.CLKIN1 := GCLK100

    mainClockCtrl.clock.load(pll.CLKOUT0)
    mainClockCtrl.reset.load(cpu_reset)

    sdramClockCtrl.clock.load(pll.CLKOUT1)
    sdramDomain.phyA.clk90.load(ClockDomain(pll.CLKOUT2))
    sdramDomain.phyA.serdesClk0.load(ClockDomain(pll.CLKOUT3))
    sdramDomain.phyA.serdesClk90.load(ClockDomain(pll.CLKOUT4))
  }

  val startupe2 = system.spiA.flash.produce(
    STARTUPE2.driveSpiClk(system.spiA.flash.sclk.setAsDirectionLess())
    //STARTUPE2.driveSpiClk(system.spiA.flash.sclk.write.setAsDirectionLess().asBools(0))
  )
}


object Arty7LinuxSystem{
  def default(g : Arty7LinuxSystem, clockCtrl : ClockDomainGenerator) = g {
    import g._

    cpu.config.load(VexRiscvConfigs.linuxTest(0xE0000000l))
    cpu.enableJtag(clockCtrl)
    cpu.hardwareBreakpointCount.load(4)

    ramA.size.load(8 KiB)
    ramA.hexInit.load(null)

    sdramA.coreParameter.load(CoreParameter(
      portTockenMin = 4,
      portTockenMax = 8,
      timingWidth = 4,
      refWidth = 16,
      writeLatencies = List(3),
      readLatencies = List(5+3, 5+4)
    ))

    uartA.parameter load UartCtrlMemoryMappedConfig(
      baudrate = 115200,
      txFifoDepth = 128,
      rxFifoDepth = 128
    )

    gpioA.parameter load Gpio.Parameter(
      width = 14,
      interrupt = List(0, 1, 2, 3)
    )
    gpioA.connectInterrupts(plic, 4)

    gpioB.parameter load Gpio.Parameter(
      width = 8
      ,output = List(0,1,2,3)
      ,input = List(4,5,6,7)
    )

    spiA.parameter load SpiXdrMasterCtrl.MemoryMappingParameters(
      SpiXdrMasterCtrl.Parameters(
        dataWidth = 8,
        timerWidth = 12,
        spi = SpiXdrParameter(
          dataWidth = 2,
          ioRate = 1,
          ssWidth = 2
        )
      ) .addFullDuplex(id = 0),
      cmdFifoDepth = 256,
      rspFifoDepth = 256
    )

    spiB.parameter load SpiXdrMasterCtrl.MemoryMappingParameters(
      SpiXdrMasterCtrl.Parameters(
        dataWidth = 8,
        timerWidth = 12,
        spi = SpiXdrParameter(
          dataWidth = 4,
          ioRate = 1,
          ssWidth = 1
        )
    ).addFullDuplex(
        id = 0
    ).addHalfDuplex(
        id=1, rate=1, ddr=false, spiWidth=2, ouputHighWhenIdle=false
    ).addHalfDuplex(
        id=2, rate=1, ddr=true, spiWidth=2, ouputHighWhenIdle=false
    ).addHalfDuplex(
        id=3, rate=1, ddr=true, spiWidth=4, ouputHighWhenIdle=false
    ),
      cmdFifoDepth = 256,
      rspFifoDepth = 256,
      cpolInit = false,
      cphaInit = false,
      modInit = 0,
      sclkToogleInit = 20,
      ssSetupInit = 20,
      ssHoldInit = 20,
      ssDisableInit = 20,
      xipConfigWritable = true,
      xipEnableInit = true,
      xipInstructionEnableInit = true,
      xipInstructionModInit = 0,
      xipAddressModInit = 3,
      xipDummyModInit = 3,
      xipPayloadModInit = 3,
      xipInstructionDataInit = 0xED,
      xipDummyCountInit = 6,
      xipDummyDataInit = 0xFF,
      xipSsId = 0
    )
    spiB.withXip.load(true)

    interconnect.addConnection(
      bridge.bmb -> List(spiB.bmb)
    )


    interconnect.setConnector(peripheralBridge.input){case (m,s) =>
      m.cmd.halfPipe >> s.cmd
      m.rsp << s.rsp.halfPipe()
    }
    interconnect.setConnector(sdramA0.bmb){case (m,s) =>
      m.cmd >/-> s.cmd
      m.rsp <-< s.rsp
    }
//    interconnect.setConnector(cpu.iBus){case (m,s) =>
//      m.cmd.halfPipe() >> s.cmd
//      m.rsp << s.rsp.halfPipe()
//    }
//    interconnect.setConnector(cpu.dBus){case (m,s) =>
//      m.cmd.halfPipe() >> s.cmd
//      m.rsp << s.rsp.halfPipe()
//    }
    interconnect.setConnector(bridge.bmb){case (m,s) =>
      m.cmd >/-> s.cmd
      m.rsp <-< s.rsp
    }
    g
  }
}


object Arty7Linux {
  //Function used to configure the SoC
  def default(g : Arty7Linux) = g{
    import g._
    mainClockCtrl.resetSensitivity.load(ResetSensitivity.LOW)
    sdramDomain.phyA.sdramLayout.load(MT41K128M16JT.layout)
    Arty7LinuxSystem.default(system, mainClockCtrl)
    system.ramA.size.load(8 KiB)
    //system.ramA.hexInit.load("software/standalone/bootloader/build/bootloader.hex")
    //system.ramA.hexInit.load("software/standalone/spiDemo/build/spiDemo.hex")
    system.ramA.hexInit.load("software/standalone/blinkAndEcho/build/blinkAndEcho.hex")
    system.cpu.produce(out(Bool).setName("inWfi") := system.cpu.config.plugins.find(_.isInstanceOf[CsrPlugin]).get.asInstanceOf[CsrPlugin].inWfi)
    g
  }

  //Generate the SoC
  def main(args: Array[String]): Unit = {
    val report = SpinalRtlConfig
      .copy(
        defaultConfigForClockDomains = ClockDomainConfig(resetKind = SYNC),
        inlineRom = true
      ).generateVerilog(InOutWrapper(default(new Arty7Linux()).toComponent()))
    BspGenerator("Arty7Linux", report.toplevel.generator, report.toplevel.generator.system.cpu.dBus)
  }
}

object Arty7LinuxSystemSim {
  import spinal.core.sim._

  def main(args: Array[String]): Unit = {

    val simConfig = SimConfig
    simConfig.allOptimisation
//    simConfig.withWave
    simConfig.addSimulatorFlag("-Wno-MULTIDRIVEN")

//    val sdcardEmulatorRtlFolder = "ext/sd_device/rtl/verilog"
//    val sdcardEmulatorFiles = List("common.v", "sd_brams.v", "sd_link.v", "sd_mgr.v", "sd_phy.v", "sd_top.v", "sd_wishbone.v")
//    sdcardEmulatorFiles.map(s => s"$sdcardEmulatorRtlFolder/$s").foreach(simConfig.addRtl(_))
//    simConfig.addSimulatorFlag(s"-I../../$sdcardEmulatorRtlFolder")
//    simConfig.addSimulatorFlag("-Wno-CASEINCOMPLETE")

    simConfig.compile(new Arty7LinuxSystem(){
      val clockCtrl = ClockDomainGenerator()
      this.onClockDomain(clockCtrl.clockDomain)
      clockCtrl.makeExternal(ResetSensitivity.HIGH)
      clockCtrl.powerOnReset.load(true)
      clockCtrl.clkFrequency.load(100 MHz)
      clockCtrl.resetHoldDuration.load(15)

      val phy = RtlPhyGenerator()
      phy.layout.load(XilinxS7Phy.phyLayout(MT41K128M16JT.layout, 2))
      phy.connect(sdramA)

      apbDecoder.addSlave(sdramA.apb, 0x100000l)

      Arty7LinuxSystem.default(this, clockCtrl)
      ramA.hexInit.load("software/standalone/bootloader/build/bootloader_spinal_sim.hex")

//      val sdcard = SdcardEmulatorGenerator()
//      sdcard.connectSpi(spiA.sdcard, spiA.sdcard.produce(spiA.sdcard.ss(0)))
    }.toComponent()).doSimUntilVoid("test", 42){dut =>
      val systemClkPeriod = (1e12/dut.clockCtrl.clkFrequency.toDouble).toLong
      val jtagClkPeriod = systemClkPeriod*4
      val uartBaudRate = 115200
      val uartBaudPeriod = (1e12/uartBaudRate).toLong

      val clockDomain = ClockDomain(dut.clockCtrl.clock, dut.clockCtrl.reset)
      clockDomain.forkStimulus(systemClkPeriod)
//      dut.sdramClockDomain.get.forkStimulus((systemClkPeriod/0.7).toInt)


      fork{
        disableSimWave()
        clockDomain.waitSampling(1000)
        waitUntil(!dut.uartA.uart.rxd.toBoolean)
        enableSimWave()
      }

      val tcpJtag = JtagTcp(
        jtag = dut.cpu.jtag,
        jtagClkPeriod = jtagClkPeriod
      )

      val uartTx = UartDecoder(
        uartPin =  dut.uartA.uart.txd,
        baudPeriod = uartBaudPeriod
      )

      val uartRx = UartEncoder(
        uartPin = dut.uartA.uart.rxd,
        baudPeriod = uartBaudPeriod
      )

//      val sdcard = SdcardEmulatorIoSpinalSim(
//        io = dut.sdcard.io,
//        nsPeriod = 1000,
//        storagePath = "/home/miaou/tmp/saxonsoc-ulx3s-bin-master/linux/images/sdimage"
//      )


      val linuxPath = "../buildroot/output/images/"
      val uboot = "../u-boot/"
      dut.phy.io.loadBin(0x01FF0000, "software/standalone/machineModeSbi/build/machineModeSbi.bin")
      dut.phy.io.loadBin(0x01F00000, uboot + "u-boot.bin")


      //      val linuxPath = "../buildroot/output/images/"
//      dut.phy.io.loadBin(0x00000000, "software/standalone/machineModeSbi/build/machineModeSbi.bin")
//      dut.phy.io.loadBin(0x00400000, linuxPath + "Image")
//      dut.phy.io.loadBin(0x00BF0000, linuxPath + "dtb")
//      dut.phy.io.loadBin(0x00C00000, linuxPath + "rootfs.cpio")

      println("DRAM loading done")

    }
  }
}



//
//object Arty7LinuxSynthesis{
//  def main(args: Array[String]): Unit = {
//    val soc = new Rtl {
//      override def getName(): String = "Arty7Linux"
//      override def getRtlPath(): String = "Arty7Linux.v"
//      SpinalConfig(defaultConfigForClockDomains = ClockDomainConfig(resetKind = SYNC), inlineRom = true)
//        .generateVerilog(InOutWrapper(Arty7Linux.default(new Arty7Linux()).toComponent()).setDefinitionName(getRtlPath().split("\\.").head))
//    }
//
//    val rtls = List(soc)
////    val targets = XilinxStdTargets(
////      vivadoArtix7Path = "/media/miaou/HD/linux/Xilinx/Vivado/2018.3/bin"
////    )
//    val targets = List(
//      new Target {
//        override def getFamilyName(): String = "Artix 7"
//        override def synthesise(rtl: Rtl, workspace: String): Report = {
//          VivadoFlow(
//            vivadoPath=vivadoArtix7Path,
//            workspacePath=workspace,
//            toplevelPath=rtl.getRtlPath(),
//            family=getFamilyName(),
//            device="xc7a35ticsg324-1L" // xc7k70t-fbg676-3"
//          )
//        }
//      }
//    )
//
//    Bench(rtls, targets, "/media/miaou/HD/linux/tmp")
//  }
//}
//
//
//
//
