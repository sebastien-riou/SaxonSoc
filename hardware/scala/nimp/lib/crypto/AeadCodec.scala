package nimp.lib.crypto


import spinal.core._
import spinal.lib._
//import spinal.lib.bus.bmb.{Bmb, BmbParameter}
import spinal.lib.bus.misc.BusSlaveFactory
//import spinal.lib.bus.simple.{PipelinedMemoryBus, PipelinedMemoryBusConfig}
import spinal.lib.fsm.{State, StateMachine}
import spinal.lib.bus.amba4.axi.{Axi4Config, Axi4ReadOnly}
import spinal.lib.bus.avalon.{AvalonMM, AvalonMMConfig}
import spinal.lib.bus.bmb.{Bmb, BmbParameter}
import spinal.lib.bus.wishbone.{Wishbone, WishboneConfig}
import spinal.lib.bus.simple._

import scala.collection.mutable.ArrayBuffer

object AeadCodecCtrl {
  def apply(p : Parameters) = new TopLevel(p)



  def main(args: Array[String]): Unit = {
    SpinalVerilog(new TopLevel(Parameters(
        src = BusParameters(addressWidth = 24, memDataWidth = 32, chunkSize = 64)
    )))
  }

  case class Parameters(src: BusParameters){
  }

  case class Config(p: Parameters) extends Bundle {
    val key = in(Bits(128 bits))
  }

  case class MemoryMappingParameters(ctrl : Parameters,
                                     cmdFifoDepth : Int = 32,
                                     rspFifoDepth : Int = 32,
                                     pipelined : Boolean = false,
                                     xip : XipBusParameters = null)

  case class XipBusParameters(addressWidth : Int,
                              lengthWidth : Int)

  def getXipBmbCapabilities() = BmbParameter(
    addressWidth  = 24,
    dataWidth     = 32,
    lengthWidth   = Int.MaxValue,
    sourceWidth   = Int.MaxValue,
    contextWidth  = Int.MaxValue,
    canRead       = true,
    canWrite      = false,
    alignment     = BmbParameter.BurstAlignement.BYTE,
    maximumPendingTransactionPerId = Int.MaxValue
  )

  case class XipCmd(p : XipBusParameters) extends Bundle {
    val address = UInt(p.addressWidth bits)
    val length = UInt(p.lengthWidth bits)
  }


  case class XipBus(p : XipBusParameters) extends Bundle with IMasterSlave{
    val cmd = Stream(XipCmd(p))
    val rsp = Stream(Fragment(Bits(32 bits)))

    override def asMaster(): Unit = {
      master(cmd)
      slave(rsp)
    }

    def fromPipelinedMemoryBus() = {
      val accessBus = new PipelinedMemoryBus(PipelinedMemoryBusConfig(24,32))
      ???
      accessBus
    }

    def fromBmb(p : BmbParameter) = {
      val bmb = Bmb(p)
      cmd.valid := bmb.cmd.valid
      cmd.address := bmb.cmd.address
      cmd.length := bmb.cmd.length
      bmb.cmd.ready := cmd.ready

      bmb.rsp.valid := rsp.valid
      bmb.rsp.data := rsp.fragment
      bmb.rsp.last := rsp.last
      bmb.rsp.source  := RegNextWhen(bmb.cmd.source,  bmb.cmd.fire)
      bmb.rsp.context := RegNextWhen(bmb.cmd.context, bmb.cmd.fire)
      bmb.rsp.setSuccess()
      rsp.ready := bmb.rsp.ready

      bmb
    }
  }

  def driveFrom(toplevel : TopLevel, bus : BusSlaveFactory, baseAddress : Int = 0)(mapping : MemoryMappingParameters) = new Area {
    import mapping._
    import toplevel.io._
    def p = toplevel.p
    require(cmdFifoDepth >= 1)
    require(rspFifoDepth >= 1)

    require(cmdFifoDepth < 32.KiB)
    require(rspFifoDepth < 32.KiB)

    //Configs
    for (i <- 0 until config.key.getWidth/32){
        bus.driveAndRead(config.key(offset=i*32,bitCount=32 bits), baseAddress+i*4) init(0)
    }

    val xip = new Area{
      val xipBus = XipBus(mapping.xip)
    }
  }


    case class BusParameters(  addressWidth : Int,
                           memDataWidth : Int,
                           chunkSize : Int){
        def burstSize = chunkSize*8/memDataWidth
        def getAxi4Config() = Axi4Config(
            addressWidth = addressWidth,
            dataWidth = memDataWidth,
            useId = false,
            useRegion = false,
            useLock = false,
            useQos = false,
            useSize = false
        )

        def getAvalonConfig() = AvalonMMConfig.bursted(
            addressWidth = addressWidth,
            dataWidth = memDataWidth,
            burstCountWidth = log2Up(burstSize + 1)).getReadOnlyConfig.copy(
            useResponse = true,
            constantBurstBehavior = true
        )

        def getPipelinedMemoryBusConfig() = PipelinedMemoryBusConfig(
            addressWidth = 32,
            dataWidth = 32
        )

        def getWishboneConfig() = WishboneConfig(
            addressWidth = 30,
            dataWidth = 32,
            selWidth = 4,
            useSTALL = false,
            useLOCK = false,
            useERR = true,
            useRTY = false,
            tgaWidth = 0,
            tgcWidth = 0,
            tgdWidth = 0,
            useBTE = true,
            useCTI = true
        )

        def getBmbParameter() = BmbParameter(
            addressWidth = 32,
            dataWidth = 32,
            lengthWidth = log2Up(this.chunkSize),
            sourceWidth = 0,
            contextWidth = 0,
            canRead = true,
            canWrite = false,
            alignment = BmbParameter.BurstAlignement.LENGTH,
            maximumPendingTransactionPerId = 1
        )
    }


  case class SrcMemCmd(p : BusParameters) extends Bundle{
    val address = UInt(p.addressWidth bit)
    val size = UInt(log2Up(log2Up(p.chunkSize) + 1) bits)
  }

  case class SrcMemRsp(p : BusParameters) extends Bundle{
    val data = Bits(p.memDataWidth bit)
    val error = Bool
  }

  case class SrcMemBus(p : BusParameters) extends Bundle with IMasterSlave{
    val cmd = Stream (SrcMemCmd(p))
    val rsp = Flow (SrcMemRsp(p))

    override def asMaster(): Unit = {
        master(cmd)
        slave(rsp)
    }

    def toAxi4ReadOnly(): Axi4ReadOnly = {
        val axiConfig = p.getAxi4Config()
        val mm = Axi4ReadOnly(axiConfig)

        mm.readCmd.valid := cmd.valid
        mm.readCmd.len := p.burstSize-1
        mm.readCmd.addr := cmd.address
        mm.readCmd.prot  := "110"
        mm.readCmd.cache := "1111"
        mm.readCmd.setBurstINCR()
        cmd.ready := mm.readCmd.ready
        rsp.valid := mm.readRsp.valid
        rsp.data  := mm.readRsp.data
        rsp.error := !mm.readRsp.isOKAY()
        mm.readRsp.ready := True
        mm
    }

    def toAvalon(): AvalonMM = {
        val avalonConfig = p.getAvalonConfig()
        val mm = AvalonMM(avalonConfig)
        mm.read := cmd.valid
        mm.burstCount := U(p.burstSize)
        mm.address := cmd.address
        cmd.ready := mm.waitRequestn
        rsp.valid := mm.readDataValid
        rsp.data := mm.readData
        rsp.error := mm.response =/= AvalonMM.Response.OKAY
        mm
    }

    def toPipelinedMemoryBus(): PipelinedMemoryBus = {
        val pipelinedMemoryBusConfig = p.getPipelinedMemoryBusConfig()
        val bus = PipelinedMemoryBus(pipelinedMemoryBusConfig)
        val counter = Counter(p.burstSize, bus.cmd.fire)
        bus.cmd.valid := cmd.valid
        bus.cmd.address := cmd.address(31 downto widthOf(counter.value) + 2) @@ counter @@ U"00"
        bus.cmd.write := False
        bus.cmd.mask.assignDontCare()
        bus.cmd.data.assignDontCare()
        cmd.ready := counter.willOverflow
        rsp.valid := bus.rsp.valid
        rsp.data := bus.rsp.payload.data
        rsp.error := False
        bus
    }

    def toWishbone(): Wishbone = {
        val wishboneConfig = p.getWishboneConfig()
        val bus = Wishbone(wishboneConfig)
        val counter = Reg(UInt(log2Up(p.burstSize) bits)) init(0)
        val pending = counter =/= 0
        val lastCycle = counter === counter.maxValue

        bus.ADR := (cmd.address >> widthOf(counter) + 2) @@ counter
        bus.CTI := lastCycle ? B"111" | B"010"
        bus.BTE := "00"
        bus.SEL := "1111"
        bus.WE  := False
        bus.DAT_MOSI.assignDontCare()
        bus.CYC := False
        bus.STB := False
        when(cmd.valid || pending){
          bus.CYC := True
          bus.STB := True
          when(bus.ACK){
            counter := counter + 1
          }
        }

        cmd.ready := cmd.valid && bus.ACK
        rsp.valid := RegNext(bus.CYC && bus.ACK) init(False)
        rsp.data := RegNext(bus.DAT_MISO)
        rsp.error := False //TODO
        bus
    }

    def toBmb() : Bmb = {
        val busParameter = p.getBmbParameter
        val bus = Bmb(busParameter)
        bus.cmd.arbitrationFrom(cmd)
        bus.cmd.opcode := Bmb.Cmd.Opcode.READ
        bus.cmd.address := cmd.address.resized
        bus.cmd.length := p.chunkSize - 1
        bus.cmd.last := True
        rsp.valid := bus.rsp.valid
        rsp.data := bus.rsp.data
        rsp.error := bus.rsp.isError
        bus.rsp.ready := True
        bus
    }
  }

  class TopLevel(val p: Parameters) extends Component {
    setDefinitionName("AeadCodecCtrl")

    val io = new Bundle {
      val config = in(Config(p))
      val mem = master(SrcMemBus(p.src))
    }
  }
}
