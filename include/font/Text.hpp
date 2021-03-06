#ifndef TEXT_H_
#define TEXT_H_

#include <engine/Context.hpp>
#include <font/Font.hpp>

#include <font/TextFormat.hpp>

#include <memory>

namespace monkeysworld {
namespace font {

/**
 *  The Text object represents the state of a block of text.
 */ 
class Text {
 public:
  /**
   *  Create a new text instance.
   *  @param font_path - the font associated with this block of text.
   */ 
  Text(engine::Context* ctx, const std::string& font_path);

  /**
   *  Changes the font associated with text.
   *  @param font_path - path to the new font.
   */ 
  void SetFont(const std::string& font_path);

  /**
   *  Returns the font object associated with this text.
   */ 
  std::shared_ptr<const Font> GetFont();

  /**
   *  Set the text associated with this text object.
   *  For now: only supports chars in range [0x30, 0x7e]
   *  @param text - the new text which should be displayed.
   */ 
  void SetText(const std::string& text);

  /**
   *  Gets the string associated with this text object.
   *  @returns underlying text string :)
   */ 
  std::string GetText() const;

  /**
   *  Sets the color of this text.
   *  @param col - new text color, in RGBA format, specified as floats in the range [0.0, 1.0]
   */ 
  void SetTextColor(const glm::vec4& col);

  /**
   *  @returns the current RGBA text color.
   */ 
  glm::vec4 GetTextColor() const;

  /**
   *  Modifies the size of outputted text.
   *  @param size_pt - the size, in pt, of the resulting glyphs.
   */ 
  void SetTextSize(float size_pt);

  /**
   *  @returns the current text size, in pt.
   */ 
  float GetTextSize() const;


  /**
   *  @returns descriptor associated with glyph atals.
   */ 
  GLuint GetTexture() const;

  void SetTextFormat(TextFormat format);

  TextFormat GetTextFormat() const {
    return format_;
  }

  /**
   *  @returns geometry corresponding with the text.
   */ 
  std::shared_ptr<model::Mesh<storage::VertexPacket2D>> GetGeometry() const;
 protected:

 private:
  engine::Context* ctx_;
  std::string text_;
  glm::vec4 color_;
  std::shared_ptr<const Font> font_;
  float size_;
  bool mesh_valid_;
  std::shared_ptr<model::Mesh<storage::VertexPacket2D>> mesh_;
  TextFormat format_;
};

}
}

#endif