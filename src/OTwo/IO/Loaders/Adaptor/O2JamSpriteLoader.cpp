#include <OTwo/IO/Loaders/Adaptor/O2JamSpriteLoader.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <SFML/System/MemoryInputStream.hpp>
#include <SFML/Graphics/Image.hpp>

#include <fmt/format.h>
#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <OTwo/IO/TextureCompiler.hpp>

namespace
{
    sf::Color LoadRGB555(std::uint16_t rgb555)
    {
        // Extract RGB555 components (5 bits each)
        const std::uint8_t r5 = (rgb555 >> 10) & 0x1F;  // bits 14-10
        const std::uint8_t g5 = (rgb555 >> 5) & 0x1F;   // bits 9-5
        const std::uint8_t b5 = rgb555 & 0x1F;          // bits 4-0

        // Convert 5-bit values to 8-bit (scale from 0-31 to 0-255)
        sf::Color result;
        result.r = (r5 * 255) / 31;
        result.g = (g5 * 255) / 31;
        result.b = (b5 * 255) / 31;

        return result;
    }

    sf::Image LoadBitmap(const std::vector<std::uint8_t>& data, unsigned int width, unsigned int height, const sf::Color& keyColor = sf::Color(255, 82, 255), const float threshold = 10.0f)
    {
        // Create pixel array for RGBA format (4 bytes per pixel)
        std::vector<std::uint8_t> pixels(width * height * 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                const int rowIndex = (y * width + x) * 2;

                // Check bounds
                if (rowIndex + 1 >= data.size())
                {
                    // Fill remaining pixels with transparent black
                    const int pixelIndex = (y * width + x) * 4;
                    pixels[pixelIndex + 0] = 0;     // R
                    pixels[pixelIndex + 1] = 0;     // G
                    pixels[pixelIndex + 2] = 0;     // B
                    pixels[pixelIndex + 3] = 0;     // A
                    continue;
                }

                // Read 16-bit RGB555 value (little endian)
                const std::uint16_t rgb555 = data[rowIndex] | (data[rowIndex + 1] << 8);

                // Extract RGB555 components (5 bits each)
                const std::uint8_t r5 = (rgb555 >> 10) & 0x1F;
                const std::uint8_t g5 = (rgb555 >> 5) & 0x1F;
                const std::uint8_t b5 = rgb555 & 0x1F;

                // Convert 5-bit values to 8-bit (scale from 0-31 to 0-255)
                const std::uint8_t r8 = (r5 * 255) / 31;
                const std::uint8_t g8 = (g5 * 255) / 31;
                const std::uint8_t b8 = (b5 * 255) / 31;

                // Set RGBA pixel
                const int pixelIndex = (y * width + x) * 4;
                pixels[pixelIndex + 0] = r8;        // R
                pixels[pixelIndex + 1] = g8;        // G
                pixels[pixelIndex + 2] = b8;        // B
                pixels[pixelIndex + 3] = 255;       // A

                // Calculate color distance for transparency
                const float distance = std::sqrt(
                    std::pow(static_cast<float>(r8 - keyColor.r), 2.f) +
                    std::pow(static_cast<float>(g8 - keyColor.g), 2.f) +
                    std::pow(static_cast<float>(b8 - keyColor.b), 2.f)
                );

                // Apply transparency based on distance to key color
                if (distance < threshold)
                {
                    // Gradually reduce alpha based on distance
                    float alpha = (distance / threshold) * 255.0f;
                    pixels[pixelIndex + 3] = static_cast<std::uint8_t>(std::min(255.0f, alpha));
                }
                else
                {
                    pixels[pixelIndex + 3] = 255;   // A (fully opaque)
                }
            }
        }

        return sf::Image({width, height}, pixels.data());
    }

    sf::Image LoadMaskBitmap(const std::vector<std::uint8_t>& data, unsigned int width, unsigned int height, const sf::Color& keyColor = sf::Color(255, 82, 255), const float threshold = 30.0f)
    {
        // Create pixel array for RGBA format, initialized to transparent black
        std::vector<std::uint8_t> pixels(width * height * 4, 0);

        // Create memory input stream
        auto stream = sf::MemoryInputStream(data.data(), data.size());

        while (stream.tell() < stream.getSize())
        {
            // Read count
            std::uint16_t count;
            if (stream.read(&count, sizeof(count)) != sizeof(count))
                break;

            if (count == 0)
                break;

            // Read x and y coordinates
            std::uint16_t x, y;
            if (stream.read(&x, sizeof(x)) != sizeof(x) ||
                stream.read(&y, sizeof(y)) != sizeof(y))
                break;

            // Check bounds
            if (x >= width || y >= height)
            {
                // Skip this chunk if coordinates are out of bounds
                auto _ = stream.seek(stream.tell().value_or(0) + count * 2);
                continue;
            }

            // Read pixel data
            std::vector<std::uint8_t> pixelData(count * 2);
            if (stream.read(pixelData.data(), count * 2) != count * 2)
                break;

            // Convert and copy pixels
            for (int i = 0; i < count; i++)
            {
                const int currentX = x + i;

                // Check if we're still within bounds
                if (currentX >= width)
                    break;

                // Read 16-bit RGB555 value (little endian)
                const int dataIndex = i * 2;
                const std::uint16_t rgb555 = pixelData[dataIndex] | (pixelData[dataIndex + 1] << 8);

                // Extract RGB555 components (5 bits each)
                const std::uint8_t r5 = (rgb555 >> 10) & 0x1F;
                const std::uint8_t g5 = (rgb555 >> 5) & 0x1F;
                const std::uint8_t b5 = rgb555 & 0x1F;

                // Convert 5-bit values to 8-bit (scale from 0-31 to 0-255)
                const std::uint8_t r8 = (r5 * 255) / 31;
                const std::uint8_t g8 = (g5 * 255) / 31;
                const std::uint8_t b8 = (b5 * 255) / 31;

                // Set RGBA pixel
                const int pixelIndex = (y * width + currentX) * 4;
                pixels[pixelIndex + 0] = r8;        // R
                pixels[pixelIndex + 1] = g8;        // G
                pixels[pixelIndex + 2] = b8;        // B
                pixels[pixelIndex + 3] = 255;       // A

                // Calculate color distance for transparency
                const float distance = std::sqrt(
                    std::pow(static_cast<float>(r8 - keyColor.r), 2.f) +
                    std::pow(static_cast<float>(g8 - keyColor.g), 2.f) +
                    std::pow(static_cast<float>(b8 - keyColor.b), 2.f)
                );

                // Apply transparency based on distance to key color
                if (distance < threshold)
                {
                    // Gradually reduce alpha based on distance
                    float alpha = (distance / threshold) * 255.0f;
                    pixels[pixelIndex + 3] = static_cast<std::uint8_t>(std::min(255.0f, alpha));
                }
                else
                {
                    pixels[pixelIndex + 3] = 255;   // A (fully opaque)
                }
            }
        }

        return sf::Image({width, height}, pixels.data());
    }

    sf::Image LoadEncodedBitmap(const std::vector<std::uint8_t>& data, unsigned int width, unsigned int height)
    {
        if (width == 0 || height == 0)
            return sf::Image();

        sf::MemoryInputStream stream(data.data(), data.size());
        auto image = sf::Image(sf::Vector2u{width, height});

        while (stream.tell() < stream.getSize())
        {
            std::uint16_t count;
            if (stream.read(&count, sizeof(count)) != sizeof(count) || count == 0)
                break;

            std::uint16_t x, y;
            if (stream.read(&x, sizeof(x)) != sizeof(x))
                break;

            if (stream.read(&y, sizeof(y)) != sizeof(y))
                break;

            for (std::size_t i = 0; i < count; i++)
            {
                std::uint16_t pixel;
                if (stream.read(&pixel, sizeof(pixel)) != sizeof(pixel))
                    break;

                uint8_t r = ((pixel >> 10) & 0x1F);
                uint8_t g = ((pixel >> 5)  & 0x1F);
                uint8_t b = (pixel & 0x1F);

                r = (r << 3) | (r >> 2);
                g = (g << 3) | (g >> 2);
                b = (b << 3) | (b >> 2);

                image.setPixel({x + i, y}, sf::Color{r, g, b, 255});
            }
        }

        return image;
    }


    void PreprocessColorKey(sf::Image& image, const sf::Color keyColor, const float threshold = 30.0f)
    {
        const sf::Vector2u size = image.getSize();

        for (unsigned int x = 0; x < size.x; x++)
        {
            for (unsigned int y = 0; y < size.y; y++)
            {
                sf::Color pixel = image.getPixel({x, y});

                // Calculate color distance
                const float distance = std::sqrt(
                    std::pow(static_cast<float>(pixel.r - keyColor.r), 2.f) +
                    std::pow(static_cast<float>(pixel.g - keyColor.g), 2.f) +
                    std::pow(static_cast<float>(pixel.b - keyColor.b), 2.f)
                );

                // Make transparent if close to key color
                if (distance < threshold)
                {
                    // Gradually reduce alpha based on distance
                    float alpha = (distance / threshold) * 255.0f;
                    pixel.a = static_cast<std::uint8_t>(std::min(255.0f, alpha));

                    image.setPixel({x, y}, pixel);
                }
            }
        }
    }
}

Gx::ResourcePtr<SpriteSheet> O2JamSpriteLoader::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
{
    const auto stream = Gx::FileSystem::Open(fileName);
    if (!stream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& inputStream = *stream.get();
    return LoadFromStream(inputStream, Gx::ResourceContext::Rebind(ctx, fileName));
}

Gx::ResourcePtr<SpriteSheet> O2JamSpriteLoader::LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const
{
    auto stream = sf::MemoryInputStream(data, size);
    return LoadFromStream(stream, ctx);
}

Gx::ResourcePtr<SpriteSheet> O2JamSpriteLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
{
    enum class TextureFormat : std::uint16_t
    {
        OJS = 0x0555,
        OJA = 0x555A,
        OJI = 0xFFF0,
        OJT = 0xFFF1,
    };

    std::uint16_t signature;
    if (stream.read(&signature, sizeof(signature)) != sizeof(signature))
        throw Gx::ResourceLoadException("Failed to read file signature");

    if (signature != 0 && signature != 1)
        throw Gx::ResourceLoadException(fmt::format("Invalid signature ({})", std::to_string(signature)));

    TextureFormat format;
    if (stream.read(&format, sizeof(format)) != sizeof(format))
        throw Gx::ResourceLoadException("Failed to read texture format");

    if (format != TextureFormat::OJS && format != TextureFormat::OJA)
        throw Gx::ResourceLoadException(fmt::format("Invalid texture format ({})", std::to_string(static_cast<std::uint16_t>(format))));

    auto fileName = ctx.GetID();
    std::transform(fileName.begin(), fileName.end(), fileName.begin(),
        [](const unsigned char c){ return std::tolower(c); });

    if (Gx::StringHelper::EndsWith(fileName, ".oji"))
        format = TextureFormat::OJI;
    else if (Gx::StringHelper::EndsWith(fileName, ".ojt"))
        format = TextureFormat::OJT;

    std::uint16_t frameCount;
    if (stream.read(&frameCount, sizeof(frameCount)) != sizeof(frameCount))
        throw Gx::ResourceLoadException("Failed to read frame count");

    if (frameCount == 0)
        return nullptr;

    std::uint16_t transCode;
    if (stream.read(&transCode, sizeof(transCode)) != sizeof(transCode))
        throw Gx::ResourceLoadException("Failed to read alpha channel");

    auto transparencyColor = sf::Color(255, 82, 255);
    if (transCode != 0)
        transparencyColor = LoadRGB555(transCode);

    const auto textureLoader = Gx::TextureLoader();
    auto frames = std::vector<sf::IntRect>();

    std::uint32_t dataOffset = stream.tell().value_or(0) + (20 * frameCount);
    auto compiler = TextureCompiler(2048);
    for (std::uint16_t i = 0; i < frameCount; i++)
    {
        sf::Rect<std::int16_t> frame;
        if (stream.read(&frame, sizeof(frame)) != sizeof(frame))
            throw Gx::ResourceLoadException("Failed to read sprite bound");

        frames.push_back(sf::IntRect{
            {frame.position.x, frame.position.y} ,
            {frame.size.x, frame.size.y}
        });

        std::uint32_t offset;
        if (stream.read(&offset, sizeof(offset)) != sizeof(offset))
            throw Gx::ResourceLoadException("Failed to read texture offset");

        offset += dataOffset;

        std::uint32_t size;
        if (stream.read(&size, sizeof(size)) != sizeof(size))
            throw Gx::ResourceLoadException("Failed to read texture size");

        std::uint32_t unk;
        if (stream.read(&unk, sizeof(unk)) != sizeof(unk))
            break;

        const std::size_t position = stream.tell().value_or(0);
        if (position == 0)
            throw Gx::ResourceLoadException("Unsupported stream");

        if (stream.seek(offset).value_or(0) != offset)
            throw Gx::ResourceLoadException("Unsupported stream");

        auto bytes = std::vector<std::uint8_t>(size);
        if (stream.read(bytes.data(), size) != size)
            throw Gx::ResourceLoadException("Failed to read texture data");

        if (stream.seek(position).value_or(0) != position)
            throw Gx::ResourceLoadException("Unsupported stream");

        auto image = sf::Image();
        if (format == TextureFormat::OJI)
            image = LoadMaskBitmap(bytes, frame.size.x, frame.size.y, transparencyColor, 20.f);
        else if (format == TextureFormat::OJA)
            image = LoadEncodedBitmap(bytes, frame.size.x, frame.size.y);
        else
            image = LoadBitmap(bytes, frame.size.x, frame.size.y, transparencyColor, 20.f);

        compiler.Add(std::move(image));
    }

    auto sheet = compiler.Compile();
    sheet.Frames = frames;

    return std::make_unique<SpriteSheet>(std::move(sheet));
}
