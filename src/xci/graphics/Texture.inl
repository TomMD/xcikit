namespace xci {
namespace graphics {


// Forward to Texture::Impl
Texture::Texture() : m_impl(new Impl) {}
Texture::~Texture() = default;
Texture::Texture(Texture&&) = default;
Texture& Texture::operator=(Texture&&) = default;

bool Texture::create(unsigned int width, unsigned int height) { return m_impl->create(width, height); }
void Texture::update(const uint8_t* pixels, const Rect_u& region) { m_impl->update(pixels, region); }

Vec2u Texture::size() const { return m_impl->size(); }


}} // namespace xci::graphics