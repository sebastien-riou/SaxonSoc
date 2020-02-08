package saxon

import spinal.core._
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3Dummy, Apb3SlaveFactory}
import spinal.lib.bus.bmb.{Bmb, BmbParameter}
import spinal.lib.bus.misc.{AddressMapping, BusSlaveFactory, SizeMapping}
import spinal.lib.com.spi.SpiHalfDuplexMaster
//import spinal.lib.com.spi.ddr.{Apb3SpiXdrMasterCtrl, SpiXdrMasterCtrl}
//import spinal.lib.com.spi.ddr.SpiXdrMasterCtrl.XipBusParameters
import nimp.lib.com.spi.{Apb3SpiXdrMasterCtrl, SpiXdrMasterCtrl}
import nimp.lib.com.spi.SpiXdrMasterCtrl.XipBusParameters
import spinal.lib.com.uart.{Apb3UartCtrl, UartCtrlGenerics, UartCtrlInitConfig, UartCtrlMemoryMappedConfig, UartParityType, UartStopType}
import spinal.lib.generator._
import spinal.lib.io.Apb3Gpio2
import spinal.lib._
import spinal.lib.misc.MachineTimer
import spinal.lib.misc.plic.{PlicGateway, PlicGatewayActiveHigh, PlicMapper, PlicMapping, PlicTarget}

import scala.collection.mutable.ArrayBuffer

object Apb3NimpSpiGenerator{
  def apply(apbOffset : BigInt, xipOffset : BigInt = 0)
           (implicit decoder: Apb3DecoderGenerator, interconnect: BmbInterconnectGenerator = null): Apb3NimpSpiGenerator ={
    new Apb3NimpSpiGenerator(apbOffset,xipOffset)
  }
}
class Apb3NimpSpiGenerator(apbOffset : Handle[BigInt] = Unset, xipOffset : Handle[BigInt] = 0)
                            (implicit decoder: Apb3DecoderGenerator, interconnect: BmbInterconnectGenerator = null) extends Generator {
  val parameter = createDependency[SpiXdrMasterCtrl.MemoryMappingParameters]
  val withXip = Handle(false)
  val interrupt = produce(logic.io.interrupt)
  val phy = produce(logic.io.spi)
  val spi = Handle[Nameable]
  val apb = produce(logic.io.apb)
  val logic = add task Apb3SpiXdrMasterCtrl(parameter.copy(xip = if(!withXip) null else XipBusParameters(24, bmbRequirements.lengthWidth)))

  val bmbRequirements = Handle[BmbParameter]
  val bmb = product[Bmb]

  dependencies += withXip.produce{
    if(withXip) {
      dependencies += bmbRequirements
      interconnect.addSlaveAt(
        capabilities = Handle(SpiXdrMasterCtrl.getXipBmbCapabilities()),
        requirements = bmbRequirements,
        bus = bmb,
        address = xipOffset
      )
      Dependable(Apb3NimpSpiGenerator.this, bmbRequirements){
        bmb.load(logic.io.xip.fromBmb(bmbRequirements))
      }
    }
  }


  dependencies += withXip

  decoder.addSlave(apb, apbOffset)

  @dontName var interruptCtrl : InterruptCtrl = null
  var interruptId = 0
  def connectInterrupt(ctrl : InterruptCtrl, id : Int): Unit = {
    ctrl.addInterrupt(interrupt, id)
    interruptCtrl = ctrl
    interruptId = id
  }

  dts(apb) {
    s"""${apb.getName()}: spi@${apbOffset.toString(16)} {
       |  compatible = "spinal-lib,spi-1.0";
       |  #address-cells = <1>;
       |  #size-cells = <0>;
       |  reg = <0x${apbOffset.toString(16)} 0x1000>;
       |}""".stripMargin
  }

  def inferSpiSdrIo() = this(Dependable(phy)(spi.load(master(phy.toSpi().setPartialName(spi, ""))))) //TODO automated naming
  def inferSpiIce40() = this(Dependable(phy)(spi.load{
    phy.toSpiIce40().asInOut().setPartialName(spi, "")
  }))
  def phyAsIo() = produceIo(phy.get)
}
