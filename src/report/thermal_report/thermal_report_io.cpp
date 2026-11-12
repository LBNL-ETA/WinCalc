#include "thermal_report.h"

#include <fileParse/FileDataHandler.hxx>

#include "thermal_report_serializers.hxx"

namespace wincalc
{
    namespace
    {
        const std::string top_node_name{"ThermalReport"};
    }

    std::string thermal_report_to_json(Thermal_Report const & report)
    {
        return Common::saveToJSONString(report, top_node_name);
    }

    std::string thermal_report_to_xml(Thermal_Report const & report)
    {
        return Common::saveToXMLString(report, top_node_name);
    }

    int save_thermal_report(Thermal_Report const & report, std::string const & file_name)
    {
        return Common::saveToFile(report, file_name, top_node_name);
    }
}   // namespace wincalc
