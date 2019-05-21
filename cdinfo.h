#ifndef CDINFO_H
#define CDINFO_H

#include <string>
#include <vector>

class CDInfo
{
    bool m_valid = false;
    int m_length_s;
    int m_track_count;
    std::string m_title;
    std::string m_discid;
    std::string m_basic_genre; // genre as reported by cddb-tool query
    std::string m_genre; // genre as reported by cddb-tool read

    std::vector<std::string> m_tracks; // track names from cddb-tool read

    std::string init_discid();
    std::string init_cddb_query(const std::string &discid_data);
    bool proc_cddb_query(const std::string &cddb_query);
    std::string init_cddb_read();
    bool proc_cddb_read(const std::string &cddb_read);

    static bool split(const std::string &line, std::string &left, std::string &right);

public:
    CDInfo(bool getTracks = true);
    bool valid() const;
    int length_s() const;
    std::string title() const;
};

#endif // CDINFO_H
