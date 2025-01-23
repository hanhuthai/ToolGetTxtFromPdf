#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <iostream>
#include <string>
#include <sys/stat.h>

bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

int main() {
    // Path to the PDF file
    std::string pdfFile = R"(D:\CODE\AI\ORC\OCRfromPDFshoes\hi.pdf)";
    pdfFile = "D:\\CODE\\AI\\ORC\\OCRfromPDFshoes\\GetTextFromPdf\\ToolGetTxtFromPdf\\MM (MATERIAL MAP) LKK48.pdf";
    pdfFile = R"(D:\CODE\AI\ORC\OCRfromPDFshoes\FW24 OZELLE\CC (COLOR CAD) LKK48 IH4714.pdf)";
   
    // Check if the file exists
    if (!file_exists(pdfFile)) {
        std::cerr << "File does not exist: " << pdfFile << std::endl;
        return 1;
    }

    // Open the PDF document
    auto doc = poppler::document::load_from_file(pdfFile);
    if (!doc) {
        std::cerr << "Cannot open PDF file.\n";
        return 1;
    }

    int total_text_boxes = 0;

    // Iterate through the pages of the document
    for (int i = 0; i < doc->pages(); ++i) {
        auto page = doc->create_page(i);
        if (!page) continue;

        // Extract text and position
        auto text_boxes = page->text_list();
        total_text_boxes += text_boxes.size();
        for (const auto& box : text_boxes) {
            std::cout << "Text: " << box.text().to_latin1() << std::endl;
            std::cout << "Position: (" << box.bbox().x() << ", " << box.bbox().y()
                << ", " << box.bbox().width() << ", " << box.bbox().height() << ")\n";
        }
    }

    std::cout << "Total number of text boxes extracted: " << total_text_boxes << std::endl;

    return 0;
}

