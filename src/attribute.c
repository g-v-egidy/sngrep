/**************************************************************************
 **
 ** sngrep - SIP Messages flow viewer
 **
 ** Copyright (C) 2013-2018 Ivan Alonso (Kaian)
 ** Copyright (C) 2013-2018 Irontec SL. All rights reserved.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
/**
 * @file attribute.c.c
 * @author Ivan Alonso [aka Kaian] <kaian@irontec.com>
 *
 * @brief Source of functions defined in attribute.h
 *
 */
#include "config.h"
#include "attribute.h"
#include "packet/dissectors/packet_sip.h"
#include "ncurses/manager.h"

static AttributeHeader attrs[ATTR_COUNT] = {
    { ATTR_CALLINDEX,   "index",       "Idx",       "Call Index",       4 , NULL },
    { ATTR_SIPFROM,     "sipfrom",     NULL,        "SIP From",         25, NULL },
    { ATTR_SIPFROMUSER, "sipfromuser", NULL,        "SIP From User",    20, NULL },
    { ATTR_SIPTO,       "sipto",       NULL,        "SIP To",           25, NULL },
    { ATTR_SIPTOUSER,   "siptouser",   NULL,        "SIP To User",      20, NULL },
    { ATTR_SRC,         "src",         NULL,        "Source",           22, NULL },
    { ATTR_DST,         "dst",         NULL,        "Destination",      22, NULL },
    { ATTR_CALLID,      "callid",      NULL,        "Call-ID",          50, NULL },
    { ATTR_XCALLID,     "xcallid",     NULL,        "X-Call-ID",        50, NULL },
    { ATTR_DATE,        "date",        NULL,        "Date",             10, NULL },
    { ATTR_TIME,        "time",        NULL,        "Time",             8, NULL },
    { ATTR_METHOD,      "method",      NULL,        "Method",           10, attr_color_sip_method },
    { ATTR_TRANSPORT,   "transport",   "Trans",     "Transport",        3, NULL },
    { ATTR_MSGCNT,      "msgcnt",      "Msgs",      "Message Count",    5, NULL },
    { ATTR_CALLSTATE,   "state",       NULL,        "Call State",       10, attr_color_call_state },
    { ATTR_CONVDUR,     "convdur",     "ConvDur",   "Conversation Duration", 7, NULL },
    { ATTR_TOTALDUR,    "totaldur",    "TotalDur",  "Total Duration",   8, NULL },
    { ATTR_REASON_TXT,  "reason",      "Reason Text",   "Reason Text",  25, NULL },
    { ATTR_WARNING,     "warning",     "Warning",   "Warning code",     4, NULL }
};

AttributeHeader *
attr_header(enum AttributeId id)
{
    return &attrs[id];
}

const gchar *
attr_description(enum AttributeId id)
{
    AttributeHeader *header;
    if ((header = attr_header(id))) {
        return header->desc;
    }
    return NULL;
}

const gchar *
attr_title(enum AttributeId id)
{
    AttributeHeader *header;
    if ((header = attr_header(id))) {
        if (header->title)
            return header->title;
        return header->desc;
    }
    return NULL;
}

const gchar *
attr_name(enum AttributeId id)
{
    AttributeHeader *header;
    if ((header = attr_header(id))) {
        return header->name;
    }
    return NULL;
}

gint
attr_width(enum AttributeId id)
{
    AttributeHeader *header;
    if ((header = attr_header(id))) {
        return header->dwidth;
    }
    return 0;
}

gint
attr_find_by_name(const gchar *name)
{
    for (guint i = 0; i < ATTR_COUNT; i++) {
        if (!strcasecmp(name, attrs[i].name)) {
            return attrs[i].id;
        }
    }
    return -1;
}

gint
attr_color(enum AttributeId id, const gchar *value)
{
    AttributeHeader *header;

    if (!setting_enabled(SETTING_CL_COLORATTR))
        return 0;

    if ((header = attr_header(id))) {
        if (header->color) {
            return header->color(value);
        }
    }
    return 0;
}

gint
attr_color_sip_method(const gchar *value)
{
    switch (sip_method_from_str(value)) {
        case SIP_METHOD_INVITE:
            return COLOR_PAIR(CP_RED_ON_DEF) | A_BOLD;
        case SIP_METHOD_NOTIFY:
            return COLOR_PAIR(CP_YELLOW_ON_DEF);
        case SIP_METHOD_OPTIONS:
            return COLOR_PAIR(CP_YELLOW_ON_DEF);
        case SIP_METHOD_REGISTER:
            return COLOR_PAIR(CP_MAGENTA_ON_DEF);
        case SIP_METHOD_SUBSCRIBE:
            return COLOR_PAIR(CP_BLUE_ON_DEF);
        default:
            return 0;
    }
}

gint
attr_color_call_state(const gchar *value)
{
    if (!strcmp(value, call_state_to_str(CALL_STATE_CALLSETUP)))
        return COLOR_PAIR(CP_YELLOW_ON_DEF);
    if (!strcmp(value, call_state_to_str(CALL_STATE_INCALL)))
        return COLOR_PAIR(CP_BLUE_ON_DEF);
    if (!strcmp(value, call_state_to_str(CALL_STATE_COMPLETED)))
        return COLOR_PAIR(CP_GREEN_ON_DEF);
    if (!strcmp(value, call_state_to_str(CALL_STATE_CANCELLED)))
        return COLOR_PAIR(CP_RED_ON_DEF);
    if (!strcmp(value, call_state_to_str(CALL_STATE_REJECTED)))
        return COLOR_PAIR(CP_RED_ON_DEF);
    if (!strcmp(value, call_state_to_str(CALL_STATE_BUSY)))
        return COLOR_PAIR(CP_MAGENTA_ON_DEF);
    if (!strcmp(value, call_state_to_str(CALL_STATE_DIVERTED)))
        return COLOR_PAIR(CP_CYAN_ON_DEF);
    return 0;
}