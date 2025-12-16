#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector> //this is a list basically

void clean_string(std::string &cleanee, const std::string &cleaner) {
  // cleanee -> name="GA1194500outer"
  // cleaner -> name="GA

  size_t start_pos = cleanee.find(cleaner);
  if (start_pos == std::string::npos) {
    cleanee.clear();
  }

  size_t end_position = start_pos + cleaner.length();
  if (end_position == std::string::npos) {
    cleanee.clear();
  }

  std::string clean_string = cleanee.erase(start_pos, end_position);
}

template <typename T> std::string to_long_string(T value) {

  std::stringstream string_stream;

  string_stream << std::setprecision(std::numeric_limits<T>::max_digits10);
  string_stream << value;

  return string_stream.str();
}

std::vector<std::string> split(const std::string &s, char seperator) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(s);

  while (std::getline(token_stream, token, seperator)) {
    tokens.push_back(token);
  }

  return tokens;
}

std::string get_max_damage(const std::string &xml_data,
                           const std::string location) {
  // the function gets a location and returns the maximum damage for that
  // position

  std::string start_tag = "<fx_gpos " + location + ">";
  std::string end_tag = "</fx_gpos>";
  std::string damage_tag = "<fx_damage_wire_pos";
  std::string damage_str;
  long double damage_number = 0.0;
  size_t pointer_position;
  size_t damage_pos;
  const size_t chunk = 32 * sizeof(char);

  size_t start_position = xml_data.find(start_tag);
  if (start_position == std::string::npos) {
    return "";
  }

  size_t end_position = xml_data.find(end_tag, start_position);
  if (end_position == std::string::npos) {
    return "";
  }

  // arranege a subspace for the real seacrh
  std::string xml_sample =
      xml_data.substr(start_position, end_position - start_position);

  pointer_position = 0;

  while (true) {
    damage_pos = xml_sample.find(damage_tag, pointer_position);
    if (damage_pos == std::string::npos) {
      break;
    }
    damage_pos +=
        damage_tag.length(); // advancing the pointer to the end of the tag
    damage_str = xml_sample.substr(damage_pos, chunk);
    clean_string(damage_str, " damage=\"");
    clean_string(damage_str, "\"");

    if (damage_number < std::stof(damage_str)) {
      damage_number = std::stof(damage_str);
    }

    pointer_position = damage_pos; // move the pointer to the damage_pos
    // std::cout << damage_str << std::endl;
  }

  damage_str = to_long_string(damage_number);

  return damage_str;
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

  size_t start_position;
  size_t closing_position;
  size_t end_position;

  while (true) {
    start_position = xml_data.find(start_tag, reader_position);
    if (start_position == std::string::npos) {
      break;
    };
    start_position += start_tag.length(); // moving the postion to contents

    closing_position = xml_data.find(">", start_position);
    if (closing_position == std::string::npos) {
      break;
    };

    single_gpos =
        xml_data.substr(start_position, closing_position - start_position);
    gpos += single_gpos;

    end_position = xml_data.find(end_tag, closing_position);
    if (end_position == std::string::npos) {
      break;
    };

    reader_position = end_position + end_tag.length();
  };

  return gpos;
}

int write_csv(std::string filename, std::vector<std::string> vector1,
              std::vector<std::string> vector2) {
  // this function takes two vectors and writes then in the a
  // csv file
  // note that this does not check the vector size for consitency

  std::ofstream file(filename);
  if (!file.is_open()) {
    return 1;
  }

  file << "Arc Length, Damage \n";

  if (vector1.size() == vector2.size()) {
    for (int i = 0; i < vector1.size(); i++) {
      file << vector1[i] + "," + vector2[i] + "\n";
    }
  } else {
    return 1;
  }

  file.close();
  return 0;
}

int main(int argc, char *argv[]) {
  // open document
  std::string filename;
  std::ifstream file;
  // std::vector<std::string> prefixes;
  std::string prefix;
  if (argc == 2) {
    filename = argv[1];
    file.open(filename);
    // handle error
    if (!file.is_open()) {
      std::cout << "Error opening the file" << std::endl;
      return 1;
    }
  } else {
    return 256;
  }

  prefix = filename.substr(0, filename.length()-4);

  // read the entire file
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();
  file.close();

  std::cout << "----------- Processing the XML file ----------" << std::endl;

  // get all damage positions available in xml file
  std::cout << "------- printing the gpos data --------" << std::endl;
  std::string gpos_2 = get_all_gpos(file_content, "fx_gpos");
  std::cout << gpos_2 << std::endl;

  std::vector<std::string> gpos_list = split(gpos_2, ' ');
  std::vector<std::string> damage_list;
  std::vector<std::string> gpos_value_list;
  for (int i = 1; i <= 8; i++) {
    std::cout << gpos_list[i] << std::endl;
    damage_list.push_back(get_max_damage(file_content, gpos_list[i]));
    clean_string(gpos_list[i], "name=");

    if (filename.find("outer") != std::string::npos) {
      clean_string(gpos_list[i], "outer");
    } else {
      clean_string(gpos_list[i], "inner");
    }

    clean_string(gpos_list[i], "\"GA");
    gpos_value_list.push_back(gpos_list[i]);
  }

  for (const auto &damages : damage_list) {
    std::cout << damages << std::endl;
  }

  write_csv(prefix + ".csv", gpos_value_list, damage_list);

  return 0;
}
