```mermaid
graph TD
    %% Package definitions
    LBNLCPPCommon[LBNLCPPCommon<br/><font color='red'>v0.14</font>]
    xmlParser[xmlParser<br/><font color='red'>feature/disable-wide-char</font>]
    FileParse[FileParse<br/><font color='red'>feature/xmlparser-no-wide-char</font>]
    miniz[miniz<br/><font color='red'>89d7a5f6c3ce8893ea042b0a9d2a2d9975589ac9</font>]
    OpticalMeasurementParser[OpticalMeasurementParser<br/><font color='red'>feature/disable-wide-char</font>]
    nlohmann[nlohmann<br/><font color='red'>v3.12.0</font>]
    BSDFXMLParser[BSDFXMLParser<br/><font color='red'>feature/disable-wide-char</font>]
    THMXParser[THMXParser<br/><font color='red'>feature/disable-wide-char</font>]
    WindowsCalcEngine[WindowsCalcEngine<br/><font color='red'>Version_1.0.66</font>]
    Windows-CalcStandards[Windows-CalcStandards<br/><font color='red'>v1.2.2</font>]
    LibraryFEMTHERM[LibraryFEMTHERM<br/><font color='red'>feature/disable-wide-char</font>]
    WinCalc[WinCalc<br/><font color='red'>feature/disable-wide-char</font>]

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
    LibraryFEMTHERM --> miniz
    LibraryFEMTHERM --> LBNLCPPCommon
    WinCalc --> LibraryFEMTHERM
```
