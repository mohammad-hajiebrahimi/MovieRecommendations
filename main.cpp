#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

// Film data structure
struct Film {
    string name;
    string director;
    string cast;
    string genre;
    double imdb;
};

// User data structure
struct User {
    string username;
    vector<string> watched; // watched movies
    vector<string> ratings; // user ratings
};

class MovieRecommender {
private:
    vector<Film> films;
    vector<User> users;
    
    // Convert rating text to number
    double ratingToNumber(const string& rating) {
        if (rating == "Perfect") return 10.0;
        if (rating == "Average") return 5.0;
        if (rating == "Poor") return -1.0;
        return 0.0;
    }
    
    // Split string by semicolon
    vector<string> splitBySemicolon(const string& str) {
        vector<string> result;
        stringstream ss(str);
        string item;
        while (getline(ss, item, ';')) {
            // Remove extra spaces
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        return result;
    }
    
public:
    // Load films from CSV file
    bool loadFilms(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening films file: " << filename << endl;
            return false;
        }
        
        string line;
        // Skip header line
        getline(file, line);
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            Film film;
            
            // Read film name
            getline(ss, film.name, ',');
            
            // Read director
            getline(ss, film.director, ',');
            
            // Read cast
            getline(ss, film.cast, ',');
            
            // Read genre
            getline(ss, film.genre, ',');
            
            // Read IMDb rating
            getline(ss, token);
            film.imdb = stod(token);
            
            films.push_back(film);
        }
        
        file.close();
        cout << "Loaded " << films.size() << " films." << endl;
        return true;
    }
    
    // Load users from CSV file
    bool loadUsers(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening users file: " << filename << endl;
            return false;
        }
        
        string line;
        // Skip header line
        getline(file, line);
        
        while (getline(file, line)) {
            stringstream ss(line);
            User user;
            
            // Read username
            getline(ss, user.username, ',');
            
            // Read watched movies
            string watched;
            getline(ss, watched, ',');
            user.watched = splitBySemicolon(watched);
            
            // Read ratings
            string ratings;
            getline(ss, ratings);
            user.ratings = splitBySemicolon(ratings);
            
            users.push_back(user);
        }
        
        file.close();
        cout << "Loaded " << users.size() << " users." << endl;
        return true;
    }
    
    // Check if user exists
    bool userExists(const string& username) {
        for (const User& user : users) {
            if (user.username == username) {
                return true;
            }
        }
        return false;
    }
    
    // Find film by name
    Film* findFilm(const string& name) {
        for (Film& film : films) {
            if (film.name == name) {
                return &film;
            }
        }
        return nullptr;
    }
    
    // Find user by username
    User* findUser(const string& username) {
        for (User& user : users) {
            if (user.username == username) {
                return &user;
            }
        }
        return nullptr;
    }
    
    // Calculate score for existing user
    double calculateScoreForExistingUser(const Film& film, const User& user) {
        double imdbScore = film.imdb * 0.7;
        double directorEffect = 0.0;
        double castEffect = 0.0;
        
        // Check watched movies from same genre
        for (size_t i = 0; i < user.watched.size(); i++) {
            Film* watchedFilm = findFilm(user.watched[i]);
            if (watchedFilm && watchedFilm->genre == film.genre) {
                double rating = ratingToNumber(user.ratings[i]);
                
                // Check director
                if (watchedFilm->director == film.director) {
                    directorEffect = rating;
                }
                
                // Check cast
                if (watchedFilm->cast == film.cast) {
                    castEffect = rating;
                }
            }
        }
        
        double directorScore = directorEffect * 0.5;
        double castScore = castEffect * 0.6;
        
        return imdbScore + directorScore + castScore;
    }
    
    // Genre recommendation
    void genreRecommendation(const string& username, const string& genre) {
        // Check valid genres
        vector<string> validGenres = {"Action", "Comedy", "Drama", "Horror", "Science Fiction"};
        bool validGenre = false;
        for (const string& g : validGenres) {
            if (g == genre) {
                validGenre = true;
                break;
            }
        }
        
        if (!validGenre) {
            cout << "Error: Invalid genre. Valid genres: Action, Comedy, Drama, Horror, Science Fiction" << endl;
            return;
        }
        
        // Collect films of requested genre
        vector<pair<Film, double>> genreFilms;
        
        if (username.empty()) {
            // New user - only based on IMDb
            for (const Film& film : films) {
                if (film.genre == genre) {
                    genreFilms.push_back({film, film.imdb});
                }
            }
        } else {
            // Check if user exists
            if (!userExists(username)) {
                cout << "Error: User " << username << " does not exist in system." << endl;
                return;
            }
            
            User* user = findUser(username);
            
            // Collect films of requested genre that user hasn't watched
            for (const Film& film : films) {
                if (film.genre == genre) {
                    // Check if user has watched this film
                    bool watched = false;
                    for (const string& watchedFilm : user->watched) {
                        if (watchedFilm == film.name) {
                            watched = true;
                            break;
                        }
                    }
                    
                    if (!watched) {
                        double score = calculateScoreForExistingUser(film, *user);
                        genreFilms.push_back({film, score});
                    }
                }
            }
        }
        
        // Sort by score (descending)
        sort(genreFilms.begin(), genreFilms.end(),
             [](const pair<Film, double>& a, const pair<Film, double>& b) {
                 if (abs(a.second - b.second) < 0.001) {
                     // If scores are equal, sort by IMDb
                     if (abs(a.first.imdb - b.first.imdb) < 0.001) {
                         // If IMDb is also equal, sort by name (ascending)
                         return a.first.name < b.first.name;
                     }
                     return a.first.imdb > b.first.imdb;
                 }
                 return a.second > b.second;
             });
        
        // Show top 3 films
        int count = min(3, (int)genreFilms.size());
        if (count == 0) {
            cout << "No films found for genre " << genre << "." << endl;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            const Film& film = genreFilms[i].first;
            cout << i + 1 << ". " << film.name << ": " << film.director 
                 << " (" << film.imdb << ")" << endl;
        }
    }
    
    // Show users list
    void showUsers() {
        cout << "\n=== Users List ===" << endl;
        for (const User& user : users) {
            cout << "Username: " << user.username << endl;
            cout << "Watched movies: ";
            for (size_t i = 0; i < user.watched.size(); i++) {
                cout << user.watched[i] << "(" << user.ratings[i] << ")";
                if (i < user.watched.size() - 1) cout << ", ";
            }
            cout << endl << endl;
        }
    }
    
    // Show films list
    void showFilms() {
        cout << "\n=== Films List ===" << endl;
        for (const Film& film : films) {
            cout << "Name: " << film.name << endl;
            cout << "Director: " << film.director << endl;
            cout << "Cast: " << film.cast << endl;
            cout << "Genre: " << film.genre << endl;
            cout << "IMDb Rating: " << film.imdb << endl;
            cout << endl;
        }
    }
};

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        cout << "Usage:" << endl;
        cout << "./ChiBebinam <path/to/users/file.csv> <path/to/films/file.csv>" << endl;
        return 1;
    }
    
    string usersFile = argv[1];
    string filmsFile = argv[2];
    
    MovieRecommender recommender;
    
    // Load data
    if (!recommender.loadFilms(filmsFile)) {
        return 1;
    }
    
    if (!recommender.loadUsers(usersFile)) {
        return 1;
    }
    
    cout << "\n=== Movie Recommendation System ===" << endl;
    cout << "Type 'exit' to quit." << endl;
    
    string command;
    while (true) {
        cout << "\nCommand: ";
        getline(cin, command);
        if (command == "exit") {
            break;
        } else if (command == "users") {
            recommender.showUsers();
        } else if (command == "films") {
            recommender.showFilms();
        } else if (command.substr(0, 20) == "genre_recommendation") {
            // Process genreRecommendation command
            size_t firstQuote = command.find("\"");
            if (firstQuote == string::npos) {
                cout << "Invalid command format." << endl;
                continue;
            }

            size_t secondQuote = command.find("\"", firstQuote + 1);
            if (secondQuote == string::npos) {
                cout << "Invalid command format." << endl;
                continue;
            }

            string userName = command.substr(firstQuote + 1, secondQuote - firstQuote - 1);

            size_t thirdQuote = command.find("\"", secondQuote + 1);
            if (thirdQuote == string::npos) {
                cout << "Invalid command format." << endl;
                continue;
            }

            size_t fourthQuote = command.find("\"", thirdQuote + 1);
            if (fourthQuote == string::npos) {
                cout << "Invalid command format." << endl;
                continue;
            }

            string genre = command.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);

            recommender.genreRecommendation(userName, genre);
        } else {
            cout << "Available commands:" << endl;
            cout << "users - Show users list" << endl;
            cout << "films - Show films list" << endl;
            cout << "genre_recommendation \"<Username>\" \"<Genre>\" - Recommend by genre" << endl;
            cout << "exit - Exit" << endl;
        }
    }
    
    return 0;
} 