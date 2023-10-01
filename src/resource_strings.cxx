#include "resource_strings.hxx"

#include <Windows.h>

#include "resource.h"

std::wstring DEFAULT_WINDOW_TEXT;
std::wstring SFC_CHECK_BAD_SURFACE_NUMBER;
std::wstring SFC_CHECK_NOT_CREATED_YET;
std::wstring DEBUG_OUTPUT_CLIPPING;
std::wstring MINIMAP;
std::wstring REGION;
std::wstring PACKING_LOG;
std::wstring MEMORY_RESERVATION;
std::wstring PROGRAM_CORRECTION;
std::wstring LOAD_GLOBAL_SETTINGS;
std::wstring GLOBAL_SETTINGS_NOT_FOUND;
std::wstring INVALID_SETTING;
std::wstring START_PACKING;
std::wstring PACKING_FOR_REGION;
std::wstring PACKAGING;
std::wstring ERROR_OCCURRED;
std::wstring SIZE_INFORMATION;
std::wstring MAP_REMAINING_CAPACITY;
std::wstring AREA_REMAINING_CAPACITY;
std::wstring CONNECTION_REMAINING_CAPACITY;
std::wstring WRITING_COURSE_INFO;
std::wstring ERROR_TEXT;
std::wstring AREA_NOT_SET;
std::wstring CHECKSUM_CORRECTION;
std::wstring START_WRITING;
std::wstring PACKING_COMPLETE;
std::wstring PACKING_ERROR;
std::wstring PACKING_ERROR_FINAL_NOTE;
std::wstring PACKING_COMPLETED_FINAL_NOTICE;
std::wstring EDIT_AREA;
std::wstring ARRANGEMENT;
std::wstring PARAMETER_SETTINGS;
std::wstring SWITCH_WITH_SPACE;
std::wstring HOLD_PARAM;
std::wstring AREA_CANNOT_BE_SET;
std::wstring INS_DEL_TAB_HELP;
std::wstring SET_BRANCH_AREA;
std::wstring CTRL_SHIFT_CLICK_HELP;
std::wstring STRING139;
std::wstring STRING140;
std::wstring STRING141;
std::wstring STRING142;
std::wstring STRING143;
std::wstring STRING144;
std::wstring STRING145;
std::wstring ON;
std::wstring OFF;
std::wstring STRING148;
std::wstring STRING149;
std::wstring STRING150;
std::wstring MAP_EDIT;
std::wstring EDIT_DIRECT;
std::wstring EDIT_BORDERS;
std::wstring STRING154;
std::wstring STRING155;
std::wstring RADIUS_SIZE_SUGGEST;
std::wstring BAND_CAPACITY_EXCEEDED;
std::wstring BLOCK_CAPACITY_EXCEEDED;
std::wstring STRING158;
std::wstring PROGRAM_ERROR;
std::wstring CONNECT_INFO_CAPACITY_EXCEEDED;
std::wstring BRANCH_AREA_LIMIT_EXCEEDED;
std::wstring INSUFFICIENT_WRITE_SPACE;
std::wstring DISJOINT_AREAS;
std::wstring STRING164;
std::wstring STRING165;
std::wstring STRING166;
std::wstring STRING167;
std::wstring STRING168;
std::wstring TMP_FONT_NAME;
std::wstring STRING170;
std::wstring STRING171;
std::wstring STRING172;
std::wstring STRING173;
std::wstring STRING174;
std::wstring STRING175;
std::wstring STRING176;
std::wstring STRING177;

static std::wstring get_resource_string(UINT uID) {
    HINSTANCE hInstance = GetModuleHandleW(NULL);

    LPWSTR str = nullptr;
    int size = LoadStringW(hInstance, uID, reinterpret_cast<LPWSTR>(&str), 0);
    return std::wstring(str, str + size);
}

void InitializeResourceStrings() {
    DEFAULT_WINDOW_TEXT = get_resource_string(IDS_DEFAULT_WINDOW_TEXT);
    SFC_CHECK_BAD_SURFACE_NUMBER =
            get_resource_string(IDS_SFC_CHECK_BAD_SURFACE_NUMBER);
    SFC_CHECK_NOT_CREATED_YET = get_resource_string(IDS_SFC_CHECK_NOT_CREATED_YET);
    DEBUG_OUTPUT_CLIPPING = get_resource_string(IDS_DEBUG_OUTPUT_CLIPPING);
    MINIMAP = get_resource_string(IDS_MINIMAP);
    REGION = get_resource_string(IDS_REGION);
    PACKING_LOG = get_resource_string(IDS_PACKING_LOG);
    MEMORY_RESERVATION = get_resource_string(IDS_MEMORY_RESERVATION);
    PROGRAM_CORRECTION = get_resource_string(IDS_PROGRAM_CORRECTION);
    LOAD_GLOBAL_SETTINGS = get_resource_string(IDS_LOAD_GLOBAL_SETTINGS);
    GLOBAL_SETTINGS_NOT_FOUND = get_resource_string(IDS_GLOBAL_SETTINGS_NOT_FOUND);
    INVALID_SETTING = get_resource_string(IDS_INVALID_SETTING);
    START_PACKING = get_resource_string(IDS_START_PACKING);
    PACKING_FOR_REGION = get_resource_string(IDS_PACKING_FOR_REGION);
    PACKAGING = get_resource_string(IDS_PACKAGING);
    ERROR_OCCURRED = get_resource_string(IDS_ERROR_OCCURRED);
    SIZE_INFORMATION = get_resource_string(IDS_SIZE_INFORMATION);
    MAP_REMAINING_CAPACITY = get_resource_string(IDS_MAP_REMAINING_CAPACITY);
    AREA_REMAINING_CAPACITY = get_resource_string(IDS_AREA_REMAINING_CAPACITY);
    CONNECTION_REMAINING_CAPACITY =
            get_resource_string(IDS_CONNECTION_REMAINING_CAPACITY);
    WRITING_COURSE_INFO = get_resource_string(IDS_WRITING_COURSE_INFO);
    ERROR_TEXT = get_resource_string(IDS_ERROR);
    AREA_NOT_SET = get_resource_string(IDS_AREA_NOT_SET);
    CHECKSUM_CORRECTION = get_resource_string(IDS_CHECKSUM_CORRECTION);
    START_WRITING = get_resource_string(IDS_START_WRITING);
    PACKING_COMPLETE = get_resource_string(IDS_PACKING_COMPLETE);
    PACKING_ERROR = get_resource_string(IDS_PACKING_ERROR);
    PACKING_ERROR_FINAL_NOTE = get_resource_string(IDS_PACKING_ERROR_FINAL_NOTE);
    PACKING_COMPLETED_FINAL_NOTICE =
            get_resource_string(IDS_PACKING_COMPLETED_FINAL_NOTICE);
    EDIT_AREA = get_resource_string(IDS_EDIT_AREA);
    ARRANGEMENT = get_resource_string(IDS_ARRANGEMENT);
    PARAMETER_SETTINGS = get_resource_string(IDS_PARAMETER_SETTINGS);
    SWITCH_WITH_SPACE = get_resource_string(IDS_SWITCH_WITH_SPACE);
    HOLD_PARAM = get_resource_string(IDS_HOLD_PARAM);
    AREA_CANNOT_BE_SET = get_resource_string(IDS_AREA_CANNOT_BE_SET);
    INS_DEL_TAB_HELP = get_resource_string(IDS_INS_DEL_TAB_HELP);
    SET_BRANCH_AREA = get_resource_string(IDS_SET_BRANCH_AREA);
    CTRL_SHIFT_CLICK_HELP = get_resource_string(IDS_CTRL_SHIFT_CLICK_HELP);
    STRING139 = get_resource_string(IDS_STRING139);
    STRING140 = get_resource_string(IDS_STRING140);
    STRING141 = get_resource_string(IDS_STRING141);
    STRING142 = get_resource_string(IDS_STRING142);
    STRING143 = get_resource_string(IDS_STRING143);
    STRING144 = get_resource_string(IDS_STRING144);
    STRING145 = get_resource_string(IDS_STRING145);
    ON = get_resource_string(IDS_ON);
    OFF = get_resource_string(IDS_OFF);
    STRING148 = get_resource_string(IDS_STRING148);
    STRING149 = get_resource_string(IDS_STRING149);
    STRING150 = get_resource_string(IDS_STRING150);
    MAP_EDIT = get_resource_string(IDS_MAP_EDIT);
    EDIT_DIRECT = get_resource_string(IDS_EDIT_DIRECT);
    EDIT_BORDERS = get_resource_string(IDS_EDIT_BORDERS);
    STRING154 = get_resource_string(IDS_STRING154);
    STRING155 = get_resource_string(IDS_STRING155);
    RADIUS_SIZE_SUGGEST = get_resource_string(IDS_RADIUS_SIZE_SUGGEST);
    BAND_CAPACITY_EXCEEDED = get_resource_string(IDS_BAND_CAPACITY_EXCEEDED);
    BLOCK_CAPACITY_EXCEEDED = get_resource_string(IDS_BLOCK_CAPACITY_EXCEEDED);
    STRING158 = get_resource_string(IDS_STRING158);
    PROGRAM_ERROR = get_resource_string(IDS_PROGRAM_ERROR);
    CONNECT_INFO_CAPACITY_EXCEEDED =
            get_resource_string(IDS_CONNECT_INFO_CAPACITY_EXCEEDED);
    BRANCH_AREA_LIMIT_EXCEEDED = get_resource_string(IDS_BRANCH_AREA_LIMIT_EXCEEDED);
    INSUFFICIENT_WRITE_SPACE = get_resource_string(IDS_INSUFFICIENT_WRITE_SPACE);
    DISJOINT_AREAS = get_resource_string(IDS_DISJOINT_AREAS);
    STRING164 = get_resource_string(IDS_STRING164);
    STRING165 = get_resource_string(IDS_STRING165);
    STRING166 = get_resource_string(IDS_STRING166);
    STRING167 = get_resource_string(IDS_STRING167);
    STRING168 = get_resource_string(IDS_STRING168);
    TMP_FONT_NAME = get_resource_string(IDS_TMP_FONT_NAME);
    STRING170 = get_resource_string(IDS_STRING170);
    STRING171 = get_resource_string(IDS_STRING171);
    STRING172 = get_resource_string(IDS_STRING172);
    STRING173 = get_resource_string(IDS_STRING173);
    STRING174 = get_resource_string(IDS_STRING174);
    STRING175 = get_resource_string(IDS_STRING175);
    STRING176 = get_resource_string(IDS_STRING176);
    STRING177 = get_resource_string(IDS_STRING177);
}