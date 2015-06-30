// don't want to use any deprecated features
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "image_search_controller.h"

namespace fs = boost::filesystem;
const string DB_PATH = "D:\\Dev\\playground\\UGuide\\database";

// copy string to FILE_NAME
void copyString(FILE_NAME des, string src)
{
	std::vector<char> writable(src.begin(), src.end());
	writable.push_back('\0');
	strcpy(des, &writable[0]);
}

int iterate_build_index(string dir, string dir_string)
{
	string child_dir_string;
	FILE_NAME child_dirs[100];	// list of child directories
	int child_dir_count = 0;
	FILE_NAME child_files[100];	// list of child files
	int child_file_count = 0;

	fs::path path = fs::system_complete(fs::path(dir));
	if (!fs::exists(path))
	{
		cout << "\nNot found: " << path.string() << endl;
		return -1;
	}

	if (fs::is_directory(path))
	{
		//if (dir.compare(DB_PATH) != 0) // not the DB root directory
		//{
		//	std::cout << dir_string << "\n";
		//}

		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_itr(path); dir_itr != end_iter; ++dir_itr)
		{
			try
			{
				if (fs::is_directory(dir_itr->status()))
				{
					child_dir_string = dir_string;
					if (dir.compare(DB_PATH) != 0) // not the DB root directory
					{
						child_dir_string += "-";
					}
					child_dir_string += dir_itr->path().filename().string();
					int result = iterate_build_index(dir_itr->path().string(), child_dir_string);
					if (result < 0)
					{
						return -1;
					}
					else
					{
						copyString(child_dirs[child_dir_count], DB_PATH + "\\" + child_dir_string);
						child_dir_count++;
					}
				}
				else if (fs::is_regular_file(dir_itr->status()))
				{
					copyString(child_files[child_file_count], dir_itr->path().string());
					child_file_count++;
				}
				else
				{
					//std::cout << dir_itr->path().filename() << " [other]\n";
				}

			}
			catch (const std::exception & ex)
			{
				std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
			}
		}
	}
	else // must be a file
	{
		std::cout << "\nFound: " << path.string() << "\n";
		return -1;
	}

	if (child_dir_count > 0 || child_file_count > 0)
	{
		ROSE_InitSDK();

		ROSE_INDEX_Ptr temp_index = NULL;

		if (child_dir_count > 0 && child_file_count > 0)
		{
			temp_index = ROSE_MergeIndex(child_dirs, child_dir_count);
			for (int i = 0; i < child_file_count; i++)
			{
				ROSE_AddToIndex(temp_index, child_files[i]);
			}
		}
		else if (child_dir_count > 0)
		{
			temp_index = ROSE_MergeIndex(child_dirs, child_dir_count);
		}
		else
		{
			try {
				temp_index = ROSE_CreateIndex(child_files, child_file_count);
			}
			catch (exception & ex)
			{
				cerr << "Exception: " << ex.what() << endl;
			}
			
		}

		ROSE_SaveIndex(temp_index, (DB_PATH + dir_string).c_str());
		ROSE_FreeIndex(temp_index);
		temp_index = NULL;

		ROSE_FinishSDK();
	}

	return 0;
}

int iterate_ls(string dir, string dir_string)
{
	string child_dir_string;

	fs::path path = fs::system_complete(fs::path(dir));
	if (!fs::exists(path))
	{
		cout << "\nNot found: " << path.string() << endl;
		return -1;
	}

	if (fs::is_directory(path))
	{
		if (dir.compare(DB_PATH) != 0) // not the DB root directory
		{
			std::cout << dir_string << "\n";
		}

		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_itr(path); dir_itr != end_iter; ++dir_itr)
		{
			try
			{
				if (fs::is_directory(dir_itr->status()))
				{
					child_dir_string = dir_string;
					if (dir.compare(DB_PATH) != 0) // not the DB root directory
					{
						child_dir_string += "-";
					}
					child_dir_string += dir_itr->path().filename().string();
					int result = iterate_ls(dir_itr->path().string(), child_dir_string);
					if (result<0) return -1;
				}
				else if (fs::is_regular_file(dir_itr->status()))
				{
					//std::cout << dir_itr->path().filename() << "\n";
				}
				else
				{
					//std::cout << dir_itr->path().filename() << " [other]\n";
				}

			}
			catch (const std::exception & ex)
			{
				std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
			}
		}
	}
	else // must be a file
	{
		std::cout << "\nFound: " << path.string() << "\n";
		return -1;
	}
	return 0;
}

string replace(string source, string key, string value)
{
	int i = 0;
	for (;;) {
		i = source.find(key, i);
		if (i == string::npos) {
			break;
		}
		source.replace(i, 1, value);
	}

	return source;
}

int build_index(string path)
{
	int result;

	if (path.compare("root") == 0)
	{
		result = iterate_build_index(DB_PATH, "");
	}
	else //TODO: need to update parent nodes
	{
		replace(path, "-", "\\");
		result = iterate_build_index(DB_PATH + path, "");
	}

	return result;
}

int list_index(string path)
{
	int result;

	if (path.compare("root") == 0)
	{
		result = iterate_ls(DB_PATH, "");
	}
	else //TODO: need to update parent nodes
	{
		replace(path, "-", "\\");
		result = iterate_ls(DB_PATH + path, "");
	}

	return result;
}