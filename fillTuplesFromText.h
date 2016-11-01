#ifndef FILLTUPLESFROMTEXT_H_
#define FILLTUPLESFROMTEXT_H_

// Externally useable functions defined in this file are
// "fillTuplesFromText" and "tupleString".

#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <sstream>
#include <climits>
#include <cstddef>
#include <cassert>

namespace Private {
    template <typename Tuple, std::size_t N>
    struct TupleFillCycler
    {
        inline static bool read(std::istream& asciiStream, Tuple* t)
        {
            bool status = false;
            if (TupleFillCycler<Tuple, N-1>::read(asciiStream, t))
            {
                asciiStream >> std::get<N-1>(*t);
                status = !asciiStream.fail();
            }
            return status;
        }
    };

    template <typename Tuple>
    struct TupleFillCycler<Tuple, 0>
    {
        inline static bool read(std::istream&, Tuple* t)
        {
            assert(t);
            return true;
        }
    };  

    template <typename Tuple, std::size_t N>
    struct TuplePrintCycler
    {
        inline static void print(std::ostream& os, const Tuple& t, const char* sep)
        {
            TuplePrintCycler<Tuple, N-1>::print(os, t, sep);
            if (N > 1 && *sep)
                os << sep;
            os << std::get<N-1>(t);
        }
    };

    template <typename Tuple>
    struct TuplePrintCycler<Tuple, 0>
    {
        inline static void print(std::ostream&, const Tuple&, const char*) {}
    };  
}


// The following function returns "true" on success, "false" on failure
template <typename Tuple>
bool fillTuplesFromText(std::istream& asciiStream,
                        std::vector<Tuple>* tofill,
                        const bool hasCommasBetweenValues = false,
                        const unsigned long maxElementsToFill = ULONG_MAX)
{
    bool status = true;
    if (maxElementsToFill)
    {
        assert(tofill);
        std::string linebuf;
        std::istringstream is;
        unsigned long nfilled = 0;
        Tuple buf;

        while (asciiStream && status && nfilled<maxElementsToFill)
        {
            std::getline(asciiStream, linebuf);
            const unsigned long len = linebuf.size();
            if (len == 0UL)
                continue;

            // Ignore lines which are pure white space
            // or which start with an arbitrary number
            // of white space characters followed by #.
            bool isComment = false;
            bool allSpace = true;
            char* line = &linebuf[0];
            for (unsigned long i=0; i<len; ++i)
            {
                // For now, we will be simply converting commas
                // into white space. Note that this is not
                // necessarily correct for non-numeric types.
                if (hasCommasBetweenValues)
                    if (line[i] == ',')
                        line[i] = ' ';
                if (isspace(line[i]))
                    continue;
                if (allSpace && line[i] == '#')
                {
                    isComment = true;
                    break;
                }
                allSpace = false;
            }
            if (isComment || allSpace)
                continue;

            // Read the data into the buffer and parse it
            is.str(linebuf);
            is.clear();
            status = Private::TupleFillCycler<Tuple,std::tuple_size<Tuple>::value>::read(is, &buf);

            // Fill the vector
            if (status)
            {
                tofill->push_back(buf);
                ++nfilled;
            }
        }
        if ((asciiStream.fail() && !asciiStream.eof()) || 
            asciiStream.bad())
            status = false;
    }
    return status;
}


// The following function returns a human-readable string representing a tuple.
// If the separator for tuple elements is not provided, single space will be used.
template <typename Tuple>
std::string tupleString(const Tuple& t, const char* separator = 0)
{
    const char defaultSep[] = " ";
    const char* sep = separator ? separator : &defaultSep[0];
    std::ostringstream os;
    Private::TuplePrintCycler<Tuple,std::tuple_size<Tuple>::value>::print(os, t, sep);
    return os.str();
}

#endif // FILLTUPLESFROMTEXT_H_
