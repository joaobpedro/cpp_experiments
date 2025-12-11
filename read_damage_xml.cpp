#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


void clean_string(std::string &cleanee, const std::string &cleaner) {
  // cleanee -> name="GA1194500inner"
  // cleaner -> name="GA
  
  size_t start_pos = cleanee.find(cleaner);
  if (start_pos == std::string::npos) {
    cleanee.clear();
  }

  size_t end_position = start_pos + cleaner.length();
  if (end_position == std::string::npos) {
    cleanee.clear();
  }

  std::string clean_string = cleanee.erase(start_pos, end_position-1);
}


std::string get_value(const std::string &xml_data, const std::string &tag) {
  // function to extract the value of a given field from tag
  std::string start_tag = "<" + tag + ">";
  std::string end_tag = "</" + tag + ">";

  size_t start_position = xml_data.find(start_tag);
  if (start_position == std::string::npos) {
    return "";
  }

  start_position += start_tag.length();

  size_t end_position = xml_data.find(end_tag, start_position);
  if (end_position == std::string::npos) {
    return "";
  }

  return xml_data.substr(start_position, end_position - start_position);
}


std::string get_attribute(const std::string &xml_data, const std::string &tag) {
  // opening and closing tags
  std::string start_tag = "<" + tag;
  // std::string end_tag = "</" + tag + ">";

  // find the tag starting
  size_t start_position = xml_data.find(start_tag);
  if (start_position == std::string::npos) {
    return "";
  }
  start_position += start_tag.length();

  // find the next closing bracket of the start tag
  size_t closing_position = xml_data.find(">", start_position);
  if (closing_position == std::string::npos) {
    return "";
  }

  return xml_data.substr(start_position, closing_position - start_position);
}


std::string get_all_gpos(const std::string &xml_data, const std::string &tag) {
  // initiating position
  size_t reader_position = 0;
  std::string gpos;
  std::string single_gpos;

  std::string start_tag = "<" + tag;
  std::string end_tag = "</" + tag + ">";

  while (true) {
    size_t start_position = xml_data.find(start_tag, reader_position);
    if (start_position == std::string::npos) {
      break;
    };
    start_position += start_tag.length(); // moving the postion to contents

    size_t closing_postion = xml_data.find(">", start_position);
    if (closing_postion == std::string::npos) {
      break;
    };

    single_gpos = xml_data.substr(start_position, closing_postion - start_position);
    // clean_string(single_gpos, "name=\"GA");
    // clean_string(single_gpos, "inner");
    gpos += single_gpos;

    size_t end_position = xml_data.find(end_tag, closing_postion);
    if( end_position == std::string::npos) {
      break;
    };

    reader_position = end_position + end_tag.length();
  };
  
  return gpos;
}


int main() {
  // open document
  std::ifstream file("./G07_TDP_intact_NOV_SN_Corrosive_inner_worst.xml");

  // handle error
  if (!file.is_open()) {
    std::cout << "Error opening the file" << std::endl;
    return 1;
  }

  // read the entire file
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();
  file.close();

  std::cout << "----------- Processing the XML file ----------" << std::endl;

  // test the processing

  std::string damage_loadcase = get_value(file_content, "fx_damage_loadcase");

  // print the data extract
  std::cout << "Printing the content of a tag" << std::endl;
  std::cout << damage_loadcase << std::endl;

  // print the atributes of a tag
  std::cout << "Printing the attributes of a tag" << std::endl;
  std::cout << get_attribute(file_content, "fx_gpos") << std::endl;

  // print all positions as test
  std::cout << "------- printing the gpos data --------" << std::endl;
  std::cout << get_all_gpos(file_content, "fx_gpos") << std::endl;

  std::cout << "This is a test" << std::endl;

  return 0;
}
