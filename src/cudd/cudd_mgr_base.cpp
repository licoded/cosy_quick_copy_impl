#include "cudd/cudd_mgr_base.hpp"
#include "cudd/part_var.hpp"
#include "cudd/formula_utils.hpp"
#include "formula/builder.hpp"
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

DdNode *ICuddMgr::transByEdgeAf(DdNode *root_ddP, Formula* edge_af)
{
    std::unordered_set<int> lit_set;
    collect_var_ids(edge_af, lit_set);
    DdNode *cur_ddP = Cudd_Ref_Wrapper(root_ddP);
    while (isXYVar(cur_ddP))
    {
        unsigned int varId = af_atomOper_vec_.at(Cudd_NodeReadIndex(cur_ddP));
        DdNode *true_addP = Cudd_IsComplement(cur_ddP) ? ADD_Not(cur_ddP) : cur_ddP;
        if (lit_set.find(varId) != lit_set.end())
            cur_ddP = Cudd_T(true_addP);
        else
            cur_ddP = Cudd_E(true_addP);
        Cudd_Unref(true_addP);
        Cudd_Ref(cur_ddP);
    }
    return cur_ddP;
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
