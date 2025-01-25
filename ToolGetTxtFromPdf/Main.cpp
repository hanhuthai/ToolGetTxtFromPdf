#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>
#include <algorithm> 

#include <iostream>
#include <string>
#include <sys/stat.h>

bool convert_pdf_to_images(const std::string& pdfFile, const std::string& outputFolder) {
	//write code convert from pdf to image use poppler
	 // Open the PDF document
	auto doc = poppler::document::load_from_file(pdfFile);
	if (!doc) {
		std::cerr << "Cannot open PDF file.\n";
		return false;
	}

	poppler::page_renderer renderer;
	// Set render hints (optional)
	renderer.set_render_hint(poppler::page_renderer::antialiasing, true);
	renderer.set_render_hint(poppler::page_renderer::text_antialiasing, true);

	for (int i = 0; i < doc->pages(); ++i) {
		poppler::page* page = doc->create_page(i);
		if (!page) continue;

		// Get the page dimensions
		//double width = page->page_rect().width();
		//double height = page->page_rect().height();

		// Render the page to an image
		//poppler::image img = renderer.render_page(page, 600.0, 600.0, 0, 0, width, height);
		poppler::image img = renderer.render_page(page, 600.0, 600.0, 0, 0);
		if (!img.is_valid()) {
			std::cerr << "Failed to render page " << i << ".\n";
			continue;
		}

		// Save the image to a file in the specified output folder
		std::string imageFile = outputFolder + "\\page_" + std::to_string(i) + ".png";
		if (!img.save(imageFile, "png")) {
			std::cerr << "Failed to save image " << imageFile << ".\n";
			continue;
		}

		std::cout << "Saved " << imageFile << std::endl;
	}

	return true;

}

bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

struct MergedTextBox {
	std::string merged_text;
	double x;
	double y;
	double width;
	double height;
};

void group_text_boxes(const std::vector<poppler::text_box>& text_boxes) {
	std::map<double, std::vector<const poppler::text_box*>> y_groups;
	for (const auto& box : text_boxes) {
		y_groups[box.bbox().y()].push_back(&box);
	}

	std::vector<MergedTextBox> merged_boxes;

	// Process each y-coordinate group
	for (const auto& y_group : y_groups) {
		const auto& boxes = y_group.second;

		// Sort boxes by x-coordinate
		std::vector<const poppler::text_box*> sorted_boxes = boxes;
		std::sort(sorted_boxes.begin(), sorted_boxes.end(), [](const poppler::text_box* a, const poppler::text_box* b) {
			return a->bbox().x() < b->bbox().x();
			});

		// Initialize the first group
		std::string current_text = sorted_boxes[0]->text().to_latin1();
		double current_x = sorted_boxes[0]->bbox().x();
		double current_y = sorted_boxes[0]->bbox().y();
		double current_width = sorted_boxes[0]->bbox().width();
		double current_height = sorted_boxes[0]->bbox().height();


		// Split into smaller groups based on x-coordinate distance
		for (size_t i = 1; i < sorted_boxes.size(); i++) {
			const poppler::text_box* current_box = sorted_boxes[i];
			const poppler::text_box* previous_box = sorted_boxes[i - 1];

			// Calculate the distance between the current box and the previous one
			double distance_x = current_box->bbox().x() - (previous_box->bbox().x() + previous_box->bbox().width());
			//std::cout << "distance_x: " << distance_x << std::endl;

			if (distance_x < 40) {
				// Merge into the current group
				current_text += " " + current_box->text().to_latin1();
				current_width = (current_box->bbox().x() + current_box->bbox().width()) - current_x;
				current_height = std::max(current_box->bbox().height(), current_height);

			}
			else {
				// Save the current group
				merged_boxes.push_back({ current_text, current_x, current_y, current_width, current_height });

				// Start a new group
				current_text = current_box->text().to_latin1();
				current_x = current_box->bbox().x();
				current_y = current_box->bbox().y();
				current_width = current_box->bbox().width();
				current_height = current_box->bbox().height();
			}
		}
		// Add the last group
		merged_boxes.push_back({ current_text, current_x, current_y, current_width, current_height });




	}
	// Print merged_boxes
	for (const auto& box : merged_boxes) {
		std::cout << "-----------------------------------\n";
		std::cout << "Merged Text: " << box.merged_text << std::endl;
		std::cout << "Position: (" << box.x << ", " << box.y
			<< ", " << box.width << ", " << box.height << ")\n";
	}
}

int main() {
	// Path to the PDF file
	std::string pdfFile = R"(D:\CODE\AI\ORC\OCRfromPDFshoes\hi.pdf)";
	pdfFile = R"(D:\CODE\AI\Shoes_Adidas_project\data\MM (MATERIAL MAP) LKK48.pdf)";
	// pdfFile = R"(D:\CODE\AI\ORC\OCRfromPDFshoes\FW24 OZELLE\CC (COLOR CAD) LKK48 IH4714.pdf)";

	 // convert_pdf_to_images function
	 //std::string outputFolder = R"(D:\CODE\AI\Shoes_Adidas_project\data\output-image)";
	 //return convert_pdf_to_images(pdfFile, outputFolder);

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
		//for (const auto& box : text_boxes) {
		//	std::cout << "Text: " << box.text().to_latin1() << std::endl;
		//	std::cout << "Position: (" << box.bbox().x() << ", " << box.bbox().y()
		//		<< ", " << box.bbox().width() << ", " << box.bbox().height() << ")\n";
		//}

		// Group and print text boxes
		group_text_boxes(text_boxes);
	}

	std::cout << "Total number of text boxes extracted: " << total_text_boxes << std::endl;

	return 0;
}

