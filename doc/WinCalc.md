```mermaid
graph TD
    %% Package definitions
    xmlParser[xmlParser<br/><font color='red'>v1.0.1</font>]
    OpticalMeasurementParser[OpticalMeasurementParser<br/><font color='red'>v2.3.1</font>]
    nlohmann[nlohmann<br/><font color='red'>v3.11.3</font>]
    BSDFXMLParser[BSDFXMLParser<br/><font color='red'>Version_0.0.1</font>]
    THMXParser[THMXParser<br/><font color='red'>v1.1.1</font>]
    WindowsCalcEngine[WindowsCalcEngine<br/><font color='red'>FixForAngularCache</font>]
    Windows-CalcStandards[Windows-CalcStandards<br/><font color='red'>v1.2.1</font>]
    WinCalc[WinCalc<br/><font color='red'>GeometryToBSDFXML</font>]

    %% Package relations
    WinCalc --> OpticalMeasurementParser
    OpticalMeasurementParser --> nlohmann
    OpticalMeasurementParser --> BSDFXMLParser
    THMXParser --> xmlParser
    WinCalc --> THMXParser    
    WinCalc --> Windows-CalcStandards
    WinCalc --> WindowsCalcEngine
