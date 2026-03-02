```mermaid
graph TD
    %% Package definitions
    LBNLCPPCommon[LBNLCPPCommon<br/><font color='red'>v0.15</font>]
    xmlParser[xmlParser<br/><font color='red'>v1.0.4</font>]
    FileParse[FileParse<br/><font color='red'>Version_1.1.1</font>]
    miniz[miniz<br/><font color='red'>89d7a5f6c3ce8893ea042b0a9d2a2d9975589ac9</font>]
    OpticalMeasurementParser[OpticalMeasurementParser<br/><font color='red'>v2.3.7</font>]
    nlohmann_omp[nlohmann_json<br/><font color='red'>v3.12.0</font>]
    nlohmann_fp[nlohmann_json<br/><font color='red'>v3.11.3</font>]
    BSDFXMLParser[BSDFXMLParser<br/><font color='red'>Version_0.0.8</font>]
    THMXParser[THMXParser<br/><font color='red'>v1.1.4</font>]
    WindowsCalcEngine[WindowsCalcEngine<br/><font color='red'>Version_1.0.67</font>]
    Windows-CalcStandards[Windows-CalcStandards<br/><font color='red'>v1.2.2</font>]
    LibraryFEMTHERM[LibraryFEMTHERM<br/><font color='red'>Version_0.16</font>]
    WinCalc[WinCalc<br/><font color='red'>v2.6.5</font>]

    %% Package relations
    WinCalc --> OpticalMeasurementParser
    OpticalMeasurementParser --> nlohmann_omp
    OpticalMeasurementParser --> BSDFXMLParser
    FileParse --> xmlParser
    FileParse --> nlohmann_fp
    BSDFXMLParser --> FileParse
    LibraryFEMTHERM --> FileParse
    THMXParser --> xmlParser
    WinCalc --> THMXParser
    WinCalc --> Windows-CalcStandards
    WinCalc --> WindowsCalcEngine
    LibraryFEMTHERM --> miniz
    LibraryFEMTHERM --> LBNLCPPCommon
    WinCalc --> LibraryFEMTHERM
```
