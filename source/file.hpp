#pragma once

#include "code_helpers.h"
#include "core.hpp"

#include <cstdio>
#include <iostream>
#include <memory>
#include <utility>
#include <filesystem>
#include <cstring>

#include "math.hpp"

/// Try to find a file by name by searching through a vector of provided directories
// This can be provided as an initializer list of strings { "/foo/bar" }
fpath
FUNCTION linux_search_file( fpath target, std::vector<fpath> search_paths )
{
    namespace fs = std::filesystem;
// Linux only call
    // Always use the executable parent directory as search reference point
    fpath self_directory = fs::canonical( "/proc/self/exe" );
    self_directory = self_directory.parent_path();

    int matches = 0;
    fpath out_path;
    fpath check_path;
    for (fpath x_path : search_paths)
    {
        check_path = x_path / target;
        std::cout << "[File] Searching for file '" << check_path << "' \n";
        if (fs::exists( check_path ))
        {
            matches = 1;
            out_path = check_path;
        }
    }
    if (matches == 0)
    {
        std::cout << "[File] No viable match found for file search " << target << "\n";
        return "";
    }
    if (matches > 1)
    {
        std::cout << "[File] Multiple candidate matches for file search " << target
                  << " . Unsure how to proceed with operation \n";
        return "";
    }
    std::cout << "[File] Candidate file found in search paths " << out_path << "\n";
    return out_path;
}

/// Read a file into the internal storage of the program
// This effectively reads a file and then returns a byte buffer repsenting the read file
// in a binary format. No attempt is made at formatting it.
byte_buffer
FUNCTION load_file_binary( fpath target )
{
    using namespace std::filesystem;
    byte_buffer out;
    FILE* tmp = nullptr;
    fuint32 tmp_filesize = 0;

    tmp = fopen( target.c_str(), "r" );
    if (tmp == nullptr)
    {
        std::cout << "[File] Failed to open file: " << target << "\n";
        std::cout << "[File] Failed to open file: " << global_database::get_primary()->project_root << "\n";
        return out;
    }
    fseek( tmp, 0, SEEK_END );
    tmp_filesize = ftell( tmp );
    // Return to beginning
    fseek( tmp, 0, SEEK_SET );

    out.resize( tmp_filesize );
    fread( out.data(), sizeof(fbyte), out.size(), tmp );
    fclose( tmp );
    std::cout << "[File] Internalized file at path: " << target << "\n";

    return out;
}
/// Deprecreated. Use the new function load_file_binary
byte_buffer
FUNCTION intern_file( fpath target ) { return load_file_binary( target ); }

/// Tests the memory layout to see if it is little endian or big endian
// Returns true if little endian
fhowdit
FUNCTION test_little_endian()
{
    fuint16 full_bits = 1;
    fuint8 first_bits = *reinterpret_cast<fuint8*>( &full_bits );
    bool little_endian = static_cast<bool>( first_bits );

    std::cout << "[File] " <<
    (little_endian ? "Platform tested for endianess, came back as little endian" :
     "Platform tested for endianess, came back as big endian") << "\n";
    return little_endian;
}

/// Returns a buffer of vertecies stored in an STL file
// NOTE In future may be interlaced with normals
std::vector<ffloat3>
FUNCTION read_stl_file( fpath target )
{
    /** STL Format
        Presumed little endian
        80 Bytes  - Header
        4 Bytes   - Number of triangles
            84th Byte - First triangle
        12 Bytes  - float3 Triangle Normal
            96th Byte - First Vertex
        12 Bytes  - float3 Vertex 1
        12 Bytes  - float3 Vertex 2
        12 Bytes  - float3 Vertex 3
        2 Bytes   - Unused attribute width
            50 Byte Stride from one Triangle to Next
    */

    byte_buffer file;
    std::vector<ffloat3> out;
    fuint32 triangle_count;
    constexpr fint32 triangle_count_byte = 80;
    constexpr fint32 first_vertex_byte = 96;
    constexpr fint32 triangle_stride = (sizeof(ffloat) * 12) + sizeof(fuint16);

    file = intern_file( target );
    bool file_read_fail = file.size() <= 0;
    if ( file_read_fail ) { return out; }

    triangle_count = *reinterpret_cast<fint32*>( triangle_count_byte + file.data() );

    out.resize( triangle_count * 3 );

    ffloat3* x_memory = 0;
    for (int i_triangle=0; i_triangle < triangle_count; ++i_triangle)
    {
        if ( (2 + i_triangle * 3) > out.size() )
        {
            std::cout << "[File] ERRROR buffer overflow whilst reading file \n";
            out.resize(0);
            return out;
        }
        // 96 is an offset to the first vertex data
        x_memory = reinterpret_cast<ffloat3*>( first_vertex_byte + file.data() +
                                               (i_triangle * triangle_stride) );
        out[ 0+ i_triangle *3 ] = *(0+ x_memory);
        out[ 1+ i_triangle *3 ] = *(1+ x_memory);
        out[ 2+ i_triangle *3 ] = *(2+ x_memory);

    }

    return out;
}

fstring
FUNCTION linux_load_text_file( fpath target, std::vector<fpath> search_paths )
{
    fpath target_path;
    byte_buffer loaded_file;
    fstring out;

    target_path = linux_search_file( target, search_paths );
    if (target_path.empty()) return "";

    loaded_file = load_file_binary( target_path );
    out.resize( loaded_file.size() );
    std::memcpy( out.data(), loaded_file.data(), loaded_file.size() );

    return out;
}
