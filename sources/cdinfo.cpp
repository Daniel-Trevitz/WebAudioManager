#include "cdinfo.h"
#include "utils/async_process.h"

#include <iostream>
#include <stdexcept>

#define CDDB_URL "http://freedb.freedb.org/~cddb/cddb.cgi"

std::string CDInfo::init_discid()
{
    async_process discid_proc("/usr/bin/cd-discid");
    discid_proc.exec();
    discid_proc.wait();
    return discid_proc.readStdOut();
}

std::string CDInfo::init_cddb_query(const std::string &discid_data)
{
    std::vector<std::string> args;

    // split discid_data into a vector for process arguments.
    const size_t len = discid_data.length();
    for (size_t j = 0, k = 0; j < len; j++)
    {
        if (discid_data[j] == ' ')
        {
            std::string ch = discid_data.substr(k, j - k);
            k = j+1;
            args.push_back(ch);
        }

        // last iteration check.
        if (j == len - 1)
        {
            std::string ch = discid_data.substr(k, j - k+1);
            args.push_back(ch);
        }
    }

    m_discid = args.at(0);

    try {
        m_track_count = std::stoi(args.at(1));
    }
    catch(...) {
        std::cerr << __PRETTY_FUNCTION__ << "Invalid track count! " << discid_data << std::endl;
        return "";
    }

    try {
        m_length_s = std::stoi(args.back());
    }
    catch(...) {
        std::cerr << __PRETTY_FUNCTION__ << "Invalid time! " << discid_data << std::endl;
        return "";
    }

    // Reverse order insert
    args.insert(args.begin(), "WebAudioManager");
    args.insert(args.begin(), "WebAudioManager");
    args.insert(args.begin(), "6");
    args.insert(args.begin(), CDDB_URL);
    args.insert(args.begin(), "query");

    async_process cddb("/usr/bin/cddb-tool", args);
    cddb.exec();
    cddb.wait();
    return cddb.readStdOut();
}

bool CDInfo::proc_cddb_query(const std::string &cddb_query)
{
    auto s1 = cddb_query.find(' ');

    if(s1 == std::string::npos)
    {
        std::cerr << __PRETTY_FUNCTION__ << " Invalid query result! " << cddb_query << std::endl;
        return false;
    }

    auto resp_code = cddb_query.substr(0, s1);
    if(resp_code == "200")
    {
        // 200 rock 3506fb05 Kasabian /
        //    |<-s1|<--s2   |<--s3
        auto s2 = cddb_query.find(' ', s1+1);
        auto s3 = cddb_query.find(' ', s2+1);
        if((s2 == std::string::npos) || (s3 == std::string::npos))
        {
            std::cerr << __PRETTY_FUNCTION__ << " Invalid query result! " << cddb_query << std::endl;
            return false;
        }

        m_basic_genre = cddb_query.substr(s1+1, s2-s1-1);
        m_title = cddb_query.substr(s3+1);
    }
    else if(resp_code == "210")
    {
        // multiple responses... blarg
        // 210 Found exact matches, list follows (until terminating `.')
        //    |<--s1                                                    |<--s2
        // misc b40d990c The Vines / Winning Days
        //     |<--s3   |<--s4                   |<--s5
        // newage b40d990c The Vines / Winning Days

        auto s2 = cddb_query.find('\n', s1+1);
        auto s3 = cddb_query.find(' ',  s2+1);
        auto s4 = cddb_query.find(' ',  s3+1);
        auto s5 = cddb_query.find('\n', s4+1);

        if((s2 == std::string::npos) || (s3 == std::string::npos) ||
                (s4 == std::string::npos) || (s5 == std::string::npos))
        {
            std::cerr << __PRETTY_FUNCTION__ << " Invalid query result! " << cddb_query << std::endl;
            return false;
        }

        m_basic_genre = cddb_query.substr(s2+1, s3-s2-1);
        m_title = cddb_query.substr(s4+1, s5-s4-1);
    }
    else
    {
        std::cerr << __PRETTY_FUNCTION__ << " Invalid query result! " << cddb_query << std::endl;
        return false;
    }

    return true;
}

std::string CDInfo::init_cddb_read()
{
    std::vector<std::string> args;
    args.push_back("read");
    args.push_back(CDDB_URL);
    args.push_back("5");
    args.push_back("WebAudioManager");
    args.push_back("WebAudioManager");
    args.push_back(m_basic_genre);
    args.push_back(m_discid);
    std::cout << m_basic_genre << " " << m_discid << std::endl;
    async_process cddbproc("/usr/bin/cddb-tool", args);
    cddbproc.exec();
    cddbproc.wait();
    return cddbproc.readStdOut();
}

bool CDInfo::proc_cddb_read(const std::string &cddb_read)
{
    std::string::size_type prev = 0;
    while(prev != std::string::npos)
    {
        auto s = cddb_read.find('\n', prev);

        auto line = cddb_read.substr(prev, s-prev-1);

        if(s != std::string::npos)
            prev = s + 1;
        else
            prev = s;

        if(line.size() < 1)
            continue;
        if(line.at(0) == '#')
            continue;

        std::string block, data;
        if(!split(line, block, data))
            continue;

        if(block == "DTITLE")
        {

        }
        else if(block == "DGENRE")
        {
            m_genre = data;
        }
        else if(block.find("TTITLE") != std::string::npos)
        {
            try {
                auto title = std::stoul(block.substr(6));
                if(title > 100)
                    throw std::out_of_range("humongous track count");
                while(m_tracks.size() < (title+1))
                    m_tracks.push_back("");
                m_tracks[title] = data;
            } catch(...) {
                std::cerr << __PRETTY_FUNCTION__ << "Invalid read!" << std::endl << cddb_read << std::endl;
                return false;
            }
        }

        if(s == std::string::npos)
            break;
    }

    return true;
}

bool CDInfo::split(const std::string &line, std::string &left, std::string &right)
{
    auto s = line.find('=');
    if(s == std::string::npos)
        return false;

    left = line.substr(0,s);
    right = line.substr(s+1);

    return true;
}

CDInfo::CDInfo(bool getTracks)
{
    std::string discid_data = init_discid();
    if(discid_data.empty())
    {
        std::cerr << __PRETTY_FUNCTION__ << " No disc id result!" << std::endl;
        return;
    }

    std::string cddb_query = init_cddb_query(discid_data);
    if(cddb_query.empty())
    {
        std::cerr << __PRETTY_FUNCTION__ << " No query result!" << std::endl;
        return;
    }

    if(!proc_cddb_query(cddb_query))
        return; // already debuged

    if(getTracks)
    {
        std::string cddb_read = init_cddb_read();
        if(cddb_read.empty())
        {
            std::cerr << __PRETTY_FUNCTION__ << " No read result!" << std::endl;
            return;
        }

        if(!proc_cddb_read(cddb_read))
            return; // already debuged
    }

    m_valid = true;
}

bool CDInfo::valid() const
{
    return m_valid;
}

int CDInfo::length_s() const
{
    return m_length_s;
}

std::string CDInfo::title() const
{
    return m_title;
}
