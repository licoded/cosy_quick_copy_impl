#include "cudd/cudd_mgr_base.hpp"
#include "cudd/part_var.hpp"
#include "formula/utils.hpp"
#include "formula/builder.hpp"
#include <range/v3/algorithm/contains.hpp>
#include <iostream>
#include <set>

namespace Cosy {

ICuddMgr::ICuddMgr(PartVar part_var, FormulaBuilder& builder)
    : part_var_(part_var), builder_(builder)
{
    createCuddMgr();
    initTrueFalseBdd();
}

ICuddMgr::~ICuddMgr()
{
    releaseMgr();
}

void ICuddMgr::releaseMgr()
{
    if (GetCuddMgr() != NULL)
    {
        if (PRINT_CUDD_FLAG && need_print_)
        {
            spdlog::critical("=== BEGIN ------ CUDD manager info === 0x{:x}", uint64_t(this));
            print_succinct_info(GetCuddMgr());
            spdlog::critical("=== END   ------ CUDD detail  info ===");
        }
        Cudd_Quit(GetCuddMgr());
    }
}

std::pair<Formula*, Formula*> *ICuddMgr::split_XY_from_edgeAf(Formula* af)
{
    return part_var_.split_XY_from_edgeAf(af, builder_);
}

void print_succinct_info(DdManager *mgr)
{
    // Create a temporary file using tmpfile (automatically deleted when closed)
    FILE *temp_file = tmpfile();
    if (!temp_file)
    {
        exit_with_error("Failed to create temporary file.");
    }
    // Redirect Cudd_PrintInfo output to the temporary file
    Cudd_PrintInfo(mgr, temp_file);
    // Rewind the temporary file to read its content
    rewind(temp_file);
    // Read and process the file content line by line
    char buffer[1024];
    int line_number = 0;
    const int start_line = 24;
    const int end_line = 44;
    const std::set<int> ignore_lines({28, 40, 42});
    while (fgets(buffer, sizeof(buffer), temp_file))
    {
        ++line_number;
        if (line_number >= start_line && line_number <= end_line)
        {
            if (ignore_lines.count(line_number) != 0)
                continue;
            std::cout << buffer; // buffer already contains a newline
        }
    }
    // Close and delete the temporary file
    fclose(temp_file);
}

} // namespace Cosy
