- Use visual studio 2022 and just run main.cpp with release mode
- Uncomment "convert_pdf_to_images" function to convert pdf to image
  . you can config dpi (300-600 is good), size of output image : renderer.render_page(page, 600.0, 600.0, 0, 0, width, height)
- Pass pdfFile to get groub text
  . If the distance between two consecutive texts on the same line is less than 40, they will be considered as one group. You can adjust this by setting "distance_x < 40"
