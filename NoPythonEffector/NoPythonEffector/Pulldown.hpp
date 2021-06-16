#pragma once
#include <Siv3D.hpp>

/**
 * 以下のコードを現行バージョンでも動くように改変
 * https://gist.github.com/Reputeless/763d992f106f416c0473a8893edcc43d
 */

class Pulldown
{
private:

	Font m_font;

	Array<String> m_items;

	size_t m_index = 0;

	Rect m_rect;

	Size m_padding{ 6, 2 };

	int32 m_downButtonSize = 16;

	bool m_opened = false;

public:

	Pulldown() = default;

	Pulldown(const Array<String>& items, const String& fontAssetName, const Point& pos = { 0,0 })
		: m_font(FontAsset(fontAssetName))
		, m_items(items)
		, m_rect(pos, 0, 0)
	{
		m_rect.h = m_font.height() + m_padding.y * 2;

		for (const auto& item : m_items)
		{
			m_rect.w = Max(m_rect.w, m_font(item).region().w);
		}

		m_rect.w += m_padding.x * 2 + m_downButtonSize;
	}

	bool isEmpty() const
	{
		return m_items.empty();
	}

	void update()
	{
		if (isEmpty())
		{
			return;
		}

		if (m_rect.leftClicked())
		{
			m_opened = !m_opened;
		}

		Point pos(m_rect.pos.movedBy(0, m_rect.h));

		if (m_opened)
		{
			const size_t num_items = m_items.size();

			for (auto i : step(num_items))
			{
				const Rect rect(pos, m_rect.w, m_rect.h);

				if (rect.leftClicked())
				{
					m_index = i;

					m_opened = false;

					break;
				}

				pos.y += m_rect.h;
			}
		}
	}

	void draw() const
	{
		m_rect.draw();

		if (isEmpty())
		{
			return;
		}

		m_rect.drawFrame(1, 0, m_opened ? Palette::Orange : Palette::Gray);

		Point pos(m_rect.pos);

		m_font(m_items[m_index]).draw(pos + m_padding, Palette::Black);

		Triangle(m_rect.x + m_rect.w - m_downButtonSize / 2 - m_padding.x, m_rect.y + m_rect.h / 2,
			m_downButtonSize * 0.5, 180_deg).draw(Palette::Black);

		pos.y += m_rect.h;

		if (m_opened)
		{
			const size_t num_items = m_items.size();

			const Rect backRect(pos, m_rect.w, m_rect.h * static_cast<int32>(num_items));

			backRect.drawShadow({ 1, 1 }, 4, 1);

			backRect.draw();

			for (const auto& item : m_items)
			{
				const Rect rect(pos, m_rect.w, m_rect.h);

				if (rect.mouseOver())
				{
					rect.draw(Palette::Skyblue);
				}

				m_font(item).draw(pos + m_padding, Palette::Black);

				pos.y += m_rect.h;
			}

			backRect.drawFrame(1, 0, Palette::Gray);
		}
	}

	void setPos(const Point& pos)
	{
		m_rect.setPos(pos);
	}

	const Rect& getRect() const
	{
		return m_rect;
	}

	size_t getIndex() const
	{
		return m_index;
	}

	String getItem() const
	{
		if (isEmpty())
		{
			return String();
		}

		return m_items[m_index];
	}
};
