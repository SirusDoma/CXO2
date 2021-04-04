#ifndef O2JAM_DIALOG_METADATA_HPP
#define O2JAM_DIALOG_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/Metadata/SpriteMetadata.hpp>
#include <O2/Metadata/UI/ButtonMetadata.hpp>
#include <O2/Metadata/UI/LabelMetadata.hpp>

struct DialogMetadata : public SpriteMetadata
{
public:
    DialogMetadata() {}
    virtual ~DialogMetadata() {}

    const ButtonMetadata &GetAcceptButton() const { return m_acceptButton; }
    void SetAcceptButton(const ButtonMetadata &acceptButton) { m_acceptButton = acceptButton; }

    const ButtonMetadata &GetCancelButton() const { return m_cancellButton; }
    void SetCancelButton(const ButtonMetadata &cancelButton) { m_cancellButton = cancelButton; }

    const LabelMetadata &GetPromptLabel() const { return m_promptLabel; }
    void SetPromptLabel(const LabelMetadata &promptLabel) { m_promptLabel = promptLabel; }

private:
    ButtonMetadata m_acceptButton, m_cancellButton;
    LabelMetadata m_promptLabel;
};

#endif