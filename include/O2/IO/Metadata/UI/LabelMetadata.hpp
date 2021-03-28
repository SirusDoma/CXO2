#ifndef LABEL_METADATA_HPP
#define LABEL_METADATA_HPP

#include <O2/IO/Metadata/TransformMetadata.hpp>

class LabelMetadata : public TransformMetadata
{
public:
    const std::string &GetString() const { return m_string; }
    void SetString(const std::string &string) { m_string = string; }

    unsigned int GetFontSize() const { return m_fontSize; }
    void SetFontSize(unsigned int fontSize) { m_fontSize = fontSize; }

private:
    std::string m_string;
    unsigned int m_fontSize;
};

#endif
