```mermaid
graph TD
    %% Package definitions
    xmlParser[xmlParser<br/><font color='red'>v1.0.3</font>]
    FileParse[FileParse<br/><font color='red'>Version_1.0.5</font>]
    OpticalMeasurementParser[OpticalMeasurementParser<br/><font color='red'>v2.3.4</font>]
    nlohmann[nlohmann<br/><font color='red'>v3.11.3</font>]
    BSDFXMLParser[BSDFXMLParser<br/><font color='red'>Version_0.0.3</font>]
    THMXParser[THMXParser<br/><font color='red'>v1.1.3</font>]
    WindowsCalcEngine[WindowsCalcEngine<br/><font color='red'>Version_1.0.54</font>]
    Windows-CalcStandards[Windows-CalcStandards<br/><font color='red'>v1.2.2</font>]
    WinCalc[WinCalc<br/><font color='red'>v2.5.4</font>]

    %% Package relations
    WinCalc --> OpticalMeasurementParser
    OpticalMeasurementParser --> nlohmann
    OpticalMeasurementParser --> BSDFXMLParser
    FileParse --> xmlParser
    BSDFXMLParser --> FileParse
    THMXParser --> xmlParser
    WinCalc --> THMXParser    
    WinCalc --> Windows-CalcStandards
    WinCalc --> WindowsCalcEngine
