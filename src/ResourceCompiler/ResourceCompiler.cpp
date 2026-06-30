#include <Genode/IO/Json.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

namespace fs = std::filesystem;

struct ResourceData
{
    std::string name;
    std::vector<std::uint8_t> data;
};

// Function to minimize JSON (remove unnecessary whitespace)
std::string minimizeJson(const Gx::Json& json)
{
    return json.dump(-1, ' ', false, nlohmann::json::error_handler_t::ignore);
}

// Function to scan directory recursively for JSON files
std::vector<fs::path> scanJsonFiles(const fs::path& directory)
{
    std::vector<fs::path> jsonFiles;

    if (!fs::exists(directory) || !fs::is_directory(directory))
    {
        std::cerr << "Directory does not exist: " << directory << std::endl;
        return jsonFiles;
    }

    for (const auto& entry: fs::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            jsonFiles.push_back(entry.path());
        }
    }

    // Sort for consistent ordering
    std::sort(jsonFiles.begin(), jsonFiles.end());

    return jsonFiles;
}

// Function to convert file path to resource name (relative to assets folder)
std::string pathToResourceName(const fs::path& filePath, const fs::path& assetsPath)
{
    auto relativePath = fs::relative(filePath, assetsPath);
    std::string resourceName = relativePath.generic_string();
    return resourceName;
}

// Function to generate C++ array declaration
std::string generateArrayDeclaration(const std::string& name, const std::vector<std::uint8_t>& data)
{
    std::ostringstream oss;
    oss << "constexpr std::array<std::uint8_t, " << data.size() << "> " << name << " =\n{\n    ";

    for (size_t i = 0; i < data.size(); ++i)
    {
        if (i > 0 && i % 16 == 0)
        {
            oss << "\n    ";
        }
        oss << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(data[i]);
        if (i < data.size() - 1)
        {
            oss << ", ";
        }
    }

    oss << "\n};\n\n";
    return oss.str();
}

// Function to sanitize variable name
std::string sanitizeVariableName(const std::string& path)
{
    std::string sanitized = path;
    std::replace(sanitized.begin(), sanitized.end(), '/', '_');
    std::replace(sanitized.begin(), sanitized.end(), '.', '_');
    std::replace(sanitized.begin(), sanitized.end(), '-', '_');
    return sanitized;
}

int main()
{
    const fs::path assetsPath = "assets";
    const fs::path controlListPath = assetsPath / "ControlList";
    const fs::path outputPath = "include/CXO2/Resources.hpp";
    
    // Additional files to include in resources (relative to assets folder)
    const std::vector<std::string> additionalFiles = {
        "Icon.png",
        "Interface/Common/Font.Bold.ttf"
    };

    std::cout << "Scanning for JSON files in: " << controlListPath << std::endl;

    auto jsonFiles = scanJsonFiles(controlListPath);
    if (jsonFiles.empty())
    {
        std::cerr << "No JSON files found in " << controlListPath << std::endl;
        return 1;
    }

    std::cout << "Found " << jsonFiles.size() << " JSON files" << std::endl;

    std::vector<ResourceData> resources;
    std::vector<std::string> arrayNames;

    // Process each JSON file
    for (const auto& jsonFile: jsonFiles)
    {
        std::cout << "Processing: " << jsonFile << std::endl;

        try
        {
            std::ifstream file(jsonFile);
            if (!file.is_open())
            {
                std::cerr << "Failed to open file: " << jsonFile << std::endl;
                continue;
            }

            Gx::Json json;
            file >> json;
            file.close();

            // Minimize JSON
            std::string minimizedJson = minimizeJson(json);

            // Convert to resource data
            ResourceData resource;
            resource.name = pathToResourceName(jsonFile, assetsPath);
            resource.data.assign(minimizedJson.begin(), minimizedJson.end());

            resources.push_back(std::move(resource));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error processing " << jsonFile << ": " << e.what() << std::endl;
            continue;
        }
    }
    
    // Process additional files
    std::cout << "Processing additional files..." << std::endl;
    for (const auto& additionalFile : additionalFiles)
    {
        fs::path filePath = assetsPath / additionalFile;
        std::cout << "Processing: " << filePath << std::endl;
        
        try
        {
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filePath << std::endl;
                continue;
            }
            
            // Read file as binary data
            file.seekg(0, std::ios::end);
            std::streamsize fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            ResourceData resource;
            resource.name = additionalFile;
            resource.data.resize(fileSize);
            
            if (!file.read(reinterpret_cast<char*>(resource.data.data()), fileSize))
            {
                std::cerr << "Failed to read file: " << filePath << std::endl;
                continue;
            }
            
            file.close();
            resources.push_back(std::move(resource));
            
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error processing " << filePath << ": " << e.what() << std::endl;
            continue;
        }
    }
    
    // Generate header file
    std::cout << "Generating header file: " << outputPath << std::endl;

    std::ofstream headerFile(outputPath);
    if (!headerFile.is_open())
    {
        std::cerr << "Failed to create header file: " << outputPath << std::endl;
        return 1;
    }

    // Write header file
    headerFile << "// Generated via \"ResourceCompiler\".\n";
    headerFile << "// Do not edit the file manually.\n\n";
    headerFile << "#pragma once\n\n";
    headerFile << "#include <array>\n";
    headerFile << "#include <unordered_map>\n";
    headerFile << "#include <string_view>\n";
    headerFile << "#include <cstdint>\n\n";
    headerFile << "namespace Cx\n";
    headerFile << "{\n";

    // Generate array declarations
    for (const auto& resource: resources)
    {
        std::string arrayName = "resource_" + sanitizeVariableName(resource.name);
        arrayNames.push_back(arrayName);

        headerFile << generateArrayDeclaration(arrayName, resource.data);
    }

    // Generate resource map
    headerFile << "// Resource map: filename -> {data_pointer, size}\n";
    headerFile << "struct ResourceEntry\n";
    headerFile << "{\n";
    headerFile << "    const std::uint8_t* data;\n";
    headerFile << "    std::size_t size;\n";
    headerFile << "};\n\n";

    headerFile << "inline const std::unordered_map<std::string_view, ResourceEntry> Resources =\n{\n";

    for (size_t i = 0; i < resources.size(); ++i)
    {
        const auto& resource = resources[i];
        const auto& arrayName = arrayNames[i];

        headerFile << "    { \"" << resource.name << "\", { "
                << arrayName << ".data(), " << arrayName << ".size() } }";

        if (i < resources.size() - 1)
        {
            headerFile << ",";
        }
        headerFile << "\n";
    }

    headerFile << "};\n\n";

    headerFile << "inline bool ContainsResource(std::string_view name)\n";
    headerFile << "{\n";
    headerFile << "    return Resources.find(name) != Resources.end();\n";
    headerFile << "}\n\n";

    headerFile << "inline const ResourceEntry* GetResource(std::string_view name)\n";
    headerFile << "{\n";
    headerFile << "    auto it = Resources.find(name);\n";
    headerFile << "    return (it != Resources.end()) ? &it->second : nullptr;\n";
    headerFile << "}\n\n";

    headerFile << "inline std::string GetResourceAsString(std::string_view name)\n";
    headerFile << "{\n";
    headerFile << "    auto* entry = GetResource(name);\n";
    headerFile << "    if (entry) {\n";
    headerFile << "        return std::string(reinterpret_cast<const char*>(entry->data), entry->size);\n";
    headerFile << "    }\n";
    headerFile << "    return {};\n";
    headerFile << "}\n\n";

    headerFile << "} // namespace Cx\n";

    headerFile.close();

    std::cout << "Successfully generated resource header with " << resources.size() << " resources" << std::endl;
    std::cout << "  - JSON files: " << (resources.size() - additionalFiles.size()) << std::endl;
    std::cout << "  - Additional files: " << additionalFiles.size() << std::endl;
    std::cout << "Header file written to: " << outputPath << std::endl;

    return 0;
}
