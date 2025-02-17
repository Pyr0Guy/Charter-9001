#include "pch.hpp"
#include "include/Sprite.hpp"
#include "include/tinyxml2.h"
#include "include/ResourceManager.hpp"

Sprite::Sprite(Vector2 pos, const std::string& filepath, const std::string& textureName, Vector2 scale, bool isAnimation, f32 animSpeed, bool loadAll)
    : m_Pos(pos), m_Scale(scale), m_MaxFrames(0), m_CurFrame(0), m_CurAnim(""), m_isOneShot(false), m_isOnPause(false), m_isAnimation(isAnimation), m_AnimSpeed(animSpeed / 60.f), m_Rotation(0)
{
    m_CurOrigin = ORIGIN::CENTER;

    {
        std::stringstream ss;
        ss << filepath << ".png";

        std::string TexturePath = ss.str();
        ResourceManager::LoadTexture2D(TexturePath, textureName);
        //m_Texture = LoadTexture(TexturePath.c_str());
        m_Texture = ResourceManager::GetTexture(textureName);
    }

    {
        if (isAnimation == true)
        {
            std::stringstream ss;
            ss << filepath << ".xml";
            m_ThatFuckingStringThatIHate = ss.str();

            if (loadAll == true)
                LoadAllAnimations(m_ThatFuckingStringThatIHate);
        }
        else
            m_CurFrameRect = { 0.f, 0.f, static_cast<float>(m_Texture.width), static_cast<float>(m_Texture.height) };
    }

    ChangeOrigin(m_CurOrigin);
}

Sprite::~Sprite()
{
    //UnloadTexture(m_Texture);
}

void Sprite::Update()
{
    if (m_isAnimation == true && m_CurAnim != "" && m_isOnPause == false)
    {
        m_CurFrame += m_AnimSpeed;

        if (m_CurFrame >= m_MaxFrames)
        {
            if (m_isOneShot == true)
            {
                m_isOnPause = true;
                m_CurFrame = m_MaxFrames - 1;
            }
            else
                m_CurFrame = 0.f;
        }

        m_CurFrameRect = m_AnimList[m_CurAnim][m_CurFrame];
        ChangeOrigin(m_CurOrigin);
    }
}

void Sprite::Draw()
{
    Rectangle dest = { m_Pos.x, m_Pos.y, m_CurFrameRect.width * m_Scale.x, m_CurFrameRect.height * m_Scale.y };
    DrawTexturePro(m_Texture, m_CurFrameRect, dest, m_Origin, m_Rotation, WHITE);
}

void Sprite::Play(const std::string& anim)
{
    if (IsAnimationExsists(anim) == false)
    {
        std::cout << "No such animation with name: " << anim << std::endl;
        return;
    }

    if (anim != m_CurAnim)
    {
        m_isOneShot = false;
        m_isOnPause = false;
        m_CurAnim = anim;
        m_CurFrame = 0;
        m_MaxFrames = m_AnimList[m_CurAnim].size();
    }
}

void Sprite::PlayOneShot(const std::string& anim)
{
    Play(anim);

    if (m_isOneShot == false)
        m_isOneShot = true;
}

void Sprite::Pause()
{
    m_isOnPause = true;
}

void Sprite::Resume()
{
    m_isOnPause = false;
}

std::string Sprite::GetCurrentAnimation() const
{
    return m_CurAnim;
}

std::vector<std::string> Sprite::GetAllAnimations() const
{
    std::vector<std::string> animations;

    for (const auto& animName : m_AnimList)
        animations.push_back(animName.first);

    return animations;
}

Vector2 Sprite::GetSpriteSize() const
{
    if (m_isAnimation == true)
        return { m_CurFrameRect.width, m_CurFrameRect.height };

    return { static_cast<float>(m_Texture.width), static_cast<float>(m_Texture.height) };
}

Vector2 Sprite::GetSpriteScale() const
{
    return m_Scale;
}

s16 Sprite::GetSpriteMaxFrames() const
{
    return m_MaxFrames;
}

int Sprite::GetSpriteCurrentFrame() const
{
    return m_CurFrame;
}

Vector2 Sprite::GetPosition() const
{
    return m_Pos;
}

void Sprite::LoadAllAnimations(std::string XMLPath)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(XMLPath.c_str());

    tinyxml2::XMLElement* rootElement = doc.FirstChildElement("TextureAtlas");

    for (tinyxml2::XMLElement* subElement = rootElement->FirstChildElement("SubTexture"); subElement != nullptr; subElement = subElement->NextSiblingElement("SubTexture"))
    {
        std::string name = subElement->Attribute("name");
        size_t start = name.find(" ") + 1;
        size_t end = name.find("0") - start;
        name = name.substr(start, end);

        const char* xPos = subElement->Attribute("x");
        int x = std::atoi(xPos);

        const char* yPos = subElement->Attribute("y");
        int y = std::atoi(yPos);

        const char* frameWidth = subElement->Attribute("width");
        int Width = std::atoi(frameWidth);

        const char* frameHeight = subElement->Attribute("height");
        int Height = std::atoi(frameHeight);

        Rectangle frame;
        frame.x = x;
        frame.y = y;
        frame.width = Width;
        frame.height = Height;

        m_AnimList[name].push_back(frame);
    }
}

void Sprite::ChangeOrigin(int origin)
{
    Vector2 texture;

    if (m_isAnimation == false)
        texture = { static_cast<float>(m_Texture.width), static_cast<float>(m_Texture.height) };
    else
        texture = { m_CurFrameRect.width, m_CurFrameRect.height };

    switch (origin)
    {
    case TOP_LEFT:
        m_Origin = { 0.f, 0.f };
        break;
    case TOP_CENTER:
        m_Origin = { ((float)texture.x / 2.f) * m_Scale.x, 0.f };
        break;
    case TOP_RIGHT:
        m_Origin = { (float)texture.x * m_Scale.x, 0.f };
        break;
    case CENTER:
        m_Origin = { ((float)texture.x / 2.f) * m_Scale.x, ((float)texture.y / 2.f) * m_Scale.y };
        break;
    case BOTTOM_LEFT:
        m_Origin = { 0.f, (float)texture.y * m_Scale.y };
        break;
    case BOTTOM_CENTER:
        m_Origin = { ((float)texture.x / 2.f) * m_Scale.x, (float)texture.y * m_Scale.y };
        break;
    case BOTTOM_RIGHT:
        m_Origin = { (float)texture.x * m_Scale.x, (float)texture.y * m_Scale.y };
        break;
    default:
        break;
    }
}

void Sprite::SetAnimationSpeed(f32 speed)
{
    m_AnimSpeed = speed;
}

void Sprite::SetOrigin(int origin)
{
    m_CurOrigin = origin;
    ChangeOrigin(m_CurOrigin);
}

void Sprite::SetPosition(Vector2 pos)
{
    m_Pos = pos;
}

void Sprite::Move(Vector2 move)
{
    m_Pos.x += move.x;
    m_Pos.y += move.y;
}

void Sprite::LoadAnimation(const std::string& anim)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(m_ThatFuckingStringThatIHate.c_str());

    tinyxml2::XMLElement* rootElement = doc.FirstChildElement("TextureAtlas");

    for (tinyxml2::XMLElement* subElement = rootElement->FirstChildElement("SubTexture"); subElement != nullptr; subElement = subElement->NextSiblingElement("SubTexture"))
    {
        std::string name = subElement->Attribute("name");
        size_t start = name.find(" ") + 1;
        size_t end = name.find("0") - start;
        name = name.substr(start, end);
        //If not found Animation name in string
        if (name.find(anim) == std::string::npos)
            continue;

        const char* xPos = subElement->Attribute("x");
        int x = std::atoi(xPos);

        const char* yPos = subElement->Attribute("y");
        int y = std::atoi(yPos);

        const char* frameWidth = subElement->Attribute("width");
        int Width = std::atoi(frameWidth);

        const char* frameHeight = subElement->Attribute("height");
        int Height = std::atoi(frameHeight);

        Rectangle frame;
        frame.x = x;
        frame.y = y;
        frame.width = Width;
        frame.height = Height;

        m_AnimList[name].push_back(frame);
    }
}

bool Sprite::IsAnimationExsists(const std::string& name) const
{
    return (m_AnimList.find(name) != m_AnimList.end());
}
