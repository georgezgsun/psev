#include <iostream>
#include <fstream>
#include <iarchiver.h>

#include <boost/filesystem.hpp>

void extract(const std::string& psevPath, const std::string& target) {
    int ret = -1;
    std::list<IArchiverFile*> fileList;

    try {
        ret = IArchiver::loadFromArchive(psevPath, &fileList);
    } catch (const std::exception& ex) {
        std::cerr << "Can't load: "<< psevPath << " - " << ex.what() << std::endl;
        return;
    }

    if (ret != 0) {
        std::cerr << "Can't load: "<< psevPath << " - unknown reason " << std::endl;
        return;
    }

    boost::filesystem::path targetDir(target);
    boost::filesystem::create_directories(targetDir);
    if (!boost::filesystem::is_directory(targetDir)) {
        std::cerr << "Can't created target directory " << target << std::endl;
        return;
    }

    std::cout << "Extracting into " << target << std::endl;

    for (const auto& item : fileList) {
        boost::filesystem::path filename = boost::filesystem::path(item->getFileName()).filename();
        targetDir /= filename;
        std::cout << filename.string() << " -- ";
        unsigned char *buffer = new unsigned char[item->getDataSize()];
        item->readData(buffer);

        std::ofstream ss(targetDir.string().c_str());
        ss.write((char*)buffer, item->getDataSize());
        ss.close();

        std::cout << "DONE" << std::endl;
        targetDir.remove_filename();
    }
}

void compress(const std::string& targetFile, const std::vector<std::string>& files, IArchiverFile::Mode mode) {
    IArchiver* archiver = GetArchiver(targetFile);

    std::cout << "Compressing into " << targetFile << std::endl;
    std::cout << "Compression level " << static_cast<int>(mode) << std::endl;
    bool status = true;
    for (auto const &file : files) {
        if (!boost::filesystem::exists(file)) {
            std::cerr << "File doesn't exist " << file << std::endl;
            status = false;
            break;
        }

        std::cout << "Adding " << file << std::endl;

        int addStatus = -1;
        if (boost::filesystem::is_directory(file)) {
            addStatus = archiver->addPath(file, mode);
        } else {
            addStatus = archiver->addFile(file, mode);
        }

        if (addStatus != 0) {
            std::cerr << "Unable to add " << file << std::endl;
            status = false;
            break;
        }
    }

    delete archiver;

    if (!status) {
        boost::filesystem::remove(targetFile);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 1 + 1) {
        std::cout << "Usage: "<< std::endl;
        std::cout << "Extract: \t" << argv[0] << " -x psev-path [target dir]" << std::endl;
        std::cout << "Compress: \t" << argv[0] << " -c[0-2] psev-path [file file dir]" << std::endl;
        std::cout << "Compression levels:" << std::endl;
        std::cout << "\t -c0/-c \t Compression and encryption (default)" << std::endl;
        std::cout << "\t -c1    \t Compression only" << std::endl;
        std::cout << "\t -c2    \t Encryption only" << std::endl;
        return 0;
    }

    std::string psevPath(argv[2]);

    if (std::string("-x").compare(argv[1]) == 0) {
        if (!boost::filesystem::exists(psevPath)) {
            std::cerr << "Cannot find " << psevPath << std::endl;

            return -1;
        }
        std::string targetDir = boost::filesystem::current_path().string();
        if (argc == 4) {
            targetDir = argv[3];
        }
        extract(psevPath, targetDir);
    } else if (std::string(argv[1]).compare(0, 2, "-c") == 0) {
        if (boost::filesystem::exists(psevPath)) {
            std::cerr << "File exists " << psevPath << std::endl;

            return -1;
        }
        IArchiverFile::Mode mode = IArchiverFile::FULL;
        switch(argv[1][2]) {
            case 0:
                break;
            case '1':
                mode = IArchiverFile::COMP_ONLY;
                break;
            case '2':
                mode = IArchiverFile::ENC_ONLY;
                break;
            default:
                std::cerr << "Unknown compression type " << argv[1][2] << std::endl;

                return -1;
        }
        std::vector<std::string> files;
        for(int i = 3; i < argc; i++) {
            files.emplace_back(std::string(argv[i]));
        }
        compress(psevPath, files, mode);
    }


    return 0;
}
