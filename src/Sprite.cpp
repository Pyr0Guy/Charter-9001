#include "pch.hpp"
#include "include/Sprite.hpp"
#include "include/tinyxml2.h"
#include "include/ResourceManager.hpp"

Sprite::Sprite(Vector2 pos, const std::string& filepath, const std::string& textureName, Vector2 scale, bool isAnimation, f32 animSpeed)
    : m_Pos(pos), m_Scale(scale), m_MaxFrames(0), m_CurFrame(0), m_CurAnim(""), m_isOneShot(false), m_isOnPause(false), m_isAnimation(isAnimation), m_AnimSpeed(animSpeed / 60.f), m_Rotation(0), m_Color(WHITE)
{
    m_CurOrigin = Origin::CENTER;

    {
        std::string TexturePath = filepath;
        ResourceManager::LoadTexture2D(TexturePath, textureName);
        m_Texture = ResourceManager::GetTexture(textureName);
    }

    if (isAnimation == true)
        m_XmlPath = filepath + ".xml";
    else
        m_CurFrameRect = { 0.f, 0.f, static_cast<float>(m_Texture.width), static_cast<float>(m_Texture.height) };

    ChangeOrigin(m_CurOrigin);
    m_CurrentAnimation = nullptr;
}

void Sprite::Update()
{
    if (!m_CurrentAnimation) return;

    if (m_isAnimation == true && m_isOnPause == false)
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

        m_CurFrameRect = (*m_CurrentAnimation)[static_cast<int>(m_CurFrame)];
        ChangeOrigin(m_CurOrigin);
    }
}

void Sprite::Draw()
{
    Rectangle dest = { m_Pos.x, m_Pos.y, m_CurFrameRect.width * m_Scale.x, m_CurFrameRect.height * m_Scale.y };
    DrawTexturePro(m_Texture, m_CurFrameRect, dest, m_Origin, m_Rotation, m_Color);
}

void Sprite::Play(const std::string& anim)
{
    if (!ResourceManager::HasAnimation(m_XmlPath, anim)) {
        ResourceManager::LoadAnimation(m_XmlPath, anim);
    }

    m_CurrentAnimation = &ResourceManager::GetAnimation(m_XmlPath, anim);
    m_MaxFrames = static_cast<s16>(m_CurrentAnimation->size());
    m_CurFrame = 0;
    m_isOneShot = false;
    m_CurAnim = anim;
}

void Sprite::PlayOneShot(const std::string& anim)
{
    Play(anim);
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

Rectangle Sprite::GetSpriteRect() const
{
    if (m_isAnimation == true)
        return { m_Pos.x, m_Pos.y, m_CurFrameRect.width * m_Scale.x, m_CurFrameRect.height * m_Scale.y };

    return { m_Pos.x, m_Pos.y, static_cast<float>(m_Texture.width) * m_Scale.x, static_cast<float>(m_Texture.height) * m_Scale.y };
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

Color Sprite::GetColor() const
{
    return m_Color;
}

Vector2 Sprite::GetSize() const
{
    return { static_cast<float>(m_Texture.width), static_cast<float>(m_Texture.height) };
}

void Sprite::ChangeOrigin(Origin origin) {
    const float width = m_CurFrameRect.width * m_Scale.x;
    const float height = m_CurFrameRect.height * m_Scale.y;

    switch (origin) {
        case Origin::TOP_LEFT:
            m_Origin = { 0, 0 };
            break;
        case Origin::TOP_CENTER:
            m_Origin = { width * 0.5f, 0 };
            break;
        case Origin::TOP_RIGHT:
            m_Origin = { width, 0 };
            break;
        case Origin::CENTER:
            m_Origin = { width * 0.5f, height * 0.5f };
            break;
        case Origin::BOTTOM_LEFT:
            m_Origin = { 0, height };
            break;
        case Origin::BOTTOM_CENTER:
            m_Origin = { width * 0.5f, height };
            break;
        case Origin::BOTTOM_RIGHT:
            m_Origin = { width, height };
            break;
    }
}

void Sprite::SetColor(const Color& c)
{
    m_Color = c;
}

void Sprite::SetAnimationSpeed(f32 speed)
{
    m_AnimSpeed = speed;
}

void Sprite::SetOrigin(Origin origin)
{
    m_CurOrigin = origin;
    ChangeOrigin(m_CurOrigin);
}

void Sprite::SetPosition(const Vector2& pos)
{
    m_Pos = pos;
}

void Sprite::SetScale(const Vector2& scale)
{
    m_Scale = scale;
    ChangeOrigin(m_CurOrigin);
}

void Sprite::SetRotation(f32 rotation)
{
    m_Rotation = rotation;
}

void Sprite::SetSize(const Vector2& size)
{
    m_Texture.width = size.x;
    m_Texture.height = size.y;
}

void Sprite::Move(const Vector2& move)
{
    m_Pos.x += move.x;
    m_Pos.y += move.y;
}

void Sprite::Rotate(f32 rotation)
{
    m_Rotation += rotation;
}
