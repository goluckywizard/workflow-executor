#include "workflow.h"

int main(int argc, char* argv[]) {
    try {
        Workflow_Parser parse(argv, argc);
    }
    catch (WorkflowException exception) {
        std::cout << "line " << exception.Line_number <<": " << exception.exception;
    }
    catch (...) {
        std::cout << "Unknown error";
    }
    return 0;
}
