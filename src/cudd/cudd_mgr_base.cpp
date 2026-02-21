#include "synutil/cudd_mgr_base.h"

namespace syn_util {

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

} // namespace syn_util
