#include "Handler.h"

#include <getopt.h>
#include <stdio.h>
#include <map>
#include <string>
#include <set>


hdlr::Handler::Handler() :
    flags_{
        {HELP_IDX,               {0, ""}},
        {INPUT_IDX,              {0, ""}},
        {OUTPUT_IDX,             {0, ""}},
        {INFO_IDX,               {0, ""}},

        {THICKNESS_IDX,          {0, ""}},
        {COLOR_IDX,              {0, ""}},
        {FILL_IDX,               {0, ""}},
        {FILL_COLOR_IDX,         {0, ""}},
        
        {CIRCLE_IDX,             {0, ""}},
        {CENTER_IDX,             {0, ""}},
        {RADIUS_IDX,             {0, ""}},
        
        {RGBFILTER_IDX,          {0, ""}},
        {COMPONENT_NAME_IDX,     {0, ""}},
        {COMPONENT_VALUE_IDX,    {0, ""}},
        
        {SPLIT_IDX,              {0, ""}},
        {NUMBER_X_IDX,           {0, ""}},
        {NUMBER_Y_IDX,           {0, ""}},
        
        {SQUARED_LINES_IDX,      {0, ""}},
        {LEFT_UP_IDX,            {0, ""}},
        {SIDE_SIZE_IDX,          {0, ""}}
    },
    short_options_("hi:o:"),
    long_options_{
        {"help",            no_argument,          NULL,    HELP_IDX},
        {"input",           required_argument,    NULL,    INPUT_IDX},
        {"output",          required_argument,    NULL,    OUTPUT_IDX},
        {"info",            no_argument,          NULL,    INFO_IDX},

        {"thickness",       required_argument,    NULL,    THICKNESS_IDX},
        {"color",           required_argument,    NULL,    COLOR_IDX},
        {"fill",            no_argument,          NULL,    FILL_IDX},
        {"fill_color",      required_argument,    NULL,    FILL_COLOR_IDX},
        
        {"circle",          no_argument,          NULL,    CIRCLE_IDX},
        {"center",          required_argument,    NULL,    CENTER_IDX},
        {"radius",          required_argument,    NULL,    RADIUS_IDX},
        
        {"rgbfilter",       no_argument,          NULL,    RGBFILTER_IDX},
        {"component_name",  required_argument,    NULL,    COMPONENT_NAME_IDX},
        {"component_value", required_argument,    NULL,    COMPONENT_VALUE_IDX},
        
        {"split",           no_argument,          NULL,    SPLIT_IDX},
        {"number_x",        required_argument,    NULL,    NUMBER_X_IDX},
        {"number_y",        required_argument,    NULL,    NUMBER_Y_IDX},
        
        {"squared_lines",   no_argument,          NULL,    SQUARED_LINES_IDX},
        {"left_up",         required_argument,    NULL,    LEFT_UP_IDX},
        {"side_size",       required_argument,    NULL,    SIDE_SIZE_IDX},
        {NULL,              0,                    NULL,    0}
    }
{}

hdlr::Handler::~Handler() = default;

void hdlr::throwError(const char *error_massage, int exit_code)
{
    printf("%s\n", error_massage);
    exit(exit_code);
}

void hdlr::Handler::getFlags(int argc, char **argv)
{    
    int option;
    while ((option = getopt_long(argc, argv, short_options_, long_options_, NULL)) != -1) {
        if (flags_.find(option) != flags_.end()) {
            flags_[option].entered = true;
            if (optarg) {
                flags_[option].parameter = optarg;
            }
        } else {
            throwError("Error: wrong flag.", HANDLER_ERROR);
        }
    }
    last_argument = argv[argc-1];
}

std::set<int> hdlr::Handler::getRedundantFlags(std::set<int>& required_flags, 
    std::set<int>& optional_flags)
{
    std::set<int> redundant_flags;

    for (auto i : flags_) {
        if (required_flags.find(i.first) == required_flags.end() && 
            optional_flags.find(i.first) == optional_flags.end()) {
            redundant_flags.insert(i.first);
        }
    }

    return redundant_flags;
}

bool hdlr::Handler::checkFlagCompliance(std::set<int>& required_flags, std::set<int>& redundant_flags) {
    bool all_required_entered = true;
    for (auto i : required_flags) {
        all_required_entered &= flags_[i].entered;
    }

    bool all_redundant_not_entered = true;
    for (auto i : redundant_flags) {
        all_redundant_not_entered &= !flags_[i].entered;
    }
    return all_required_entered && all_redundant_not_entered;
}

void hdlr::Handler::getFinFoutNames(std::string &input_file_name, std::string &output_file_name)
{
    if (flags_[INPUT_IDX].entered) {
        input_file_name = flags_[INPUT_IDX].parameter;
    } else {
        input_file_name = last_argument;
    }

    if (flags_[OUTPUT_IDX].entered) {
        output_file_name = flags_[OUTPUT_IDX].parameter;
    }
}

void hdlr::Handler::handleFlags()
{
    if (isHelp()) {
        doHelp();
    } else if (isInfo()) {
        doInfo();
    } else if (isCircle()) {
        doCircle();
    } else if (isRGBFilter()) {
        doRGBFilter();
    } else if (isSplit()) {
        doSplit();
    } else if (isSquaredLines()) {
        doSquaredLines();
    } else {
        throwError("Error: invalid set of flags.", HANDLER_ERROR);
    }
}