```mermaid
graph TD
    %% Package definitions
    xmlParser[xmlParser<br/><font color='red'>v1.0.2</font>]
    FileParse[FileParse<br/><font color='red'>Version_1.0.4</font>]
    OpticalMeasurementParser[OpticalMeasurementParser<br/><font color='red'>v2.3.3</font>]
    nlohmann[nlohmann<br/><font color='red'>v3.11.3</font>]
    BSDFXMLParser[BSDFXMLParser<br/><font color='red'>Version_0.0.2</font>]
    THMXParser[THMXParser<br/><font color='red'>v1.1.2</font>]
    WindowsCalcEngine[WindowsCalcEngine<br/><font color='red'>Version_1.0.51</font>]
    Windows-CalcStandards[Windows-CalcStandards<br/><font color='red'>v1.2.1</font>]
    WinCalc[WinCalc<br/><font color='red'>feb_2025_productdata_composition_refactorv</font>]

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
