#ifndef __ROSEWOOD_DATA_FORMAT_READER_H__
#define __ROSEWOOD_DATA_FORMAT_READER_H__

#include <string>

namespace rosewood { namespace data_format {

    struct Object;

    Object read_data(const std::string &source);

} }

#endif
