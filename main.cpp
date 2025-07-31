#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

struct Film {
    string name;
    string director;
    string cast;
    string genre;
    double imdb;
};

struct User {
    string username;
    vector<string> watched;
    vector<string> ratings;
};

class MovieRecommender {
private:
    vector<Film> films;
    vector<User> users;

    double ratingToDirectorEffect(const string& rating) {
        if (rating == "Perfect") return 10.0;
        if (rating == "Average") return 5.0;
        if (rating == "Poor") return -1.0;
        return 0.0;
    }

    double ratingToCastEffect(const string& rating) {
        if (rating == "Perfect") return 7.0;
        if (rating == "Average") return 4.0;
        if (rating == "Poor") return -2.0;
        return 0.0;
    }

    vector<string> splitBySemicolon(const string& str) {
        vector<string> result;
        stringstream ss(str);
        string item;
        while (getline(ss, item, ';')) {
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        return result;
    }

public:
    bool loadFilms(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        getline(file, line); // skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            Film film;

            getline(ss, film.name, ',');
            getline(ss, film.director, ',');
            getline(ss, film.cast, ',');
            getline(ss, film.genre, ',');
            getline(ss, token);
            film.imdb = stod(token);

            films.push_back(film);
        }

        return true;
    }

    bool loadUsers(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        getline(file, line); // skip header

        while (getline(file, line)) {
            stringstream ss(line);
            User user;

            getline(ss, user.username, ',');

            string watched;
            getline(ss, watched, ',');
            user.watched = splitBySemicolon(watched);

            string ratings;
            getline(ss, ratings);
            user.ratings = splitBySemicolon(ratings);

            users.push_back(user);
        }

        return true;
    }

    Film* findFilm(const string& name) {
        for (auto& film : films)
            if (film.name == name) return &film;
        return nullptr;
    }

    User* findUser(const string& username) {
        for (auto& user : users)
            if (user.username == username) return &user;
        return nullptr;
    }

    double calculateScoreForExistingUser(const Film& film, const User& user) {
        double imdbScore = film.imdb * 0.7;
        double directorEffect = 0.0;
        double castEffect = 0.0;

        for (size_t i = 0; i < user.watched.size(); ++i) {
            Film* f = findFilm(user.watched[i]);
            if (!f || f->genre != film.genre) continue;

            string rating = user.ratings[i];

            if (f->director == film.director)
                directorEffect = ratingToDirectorEffect(rating);

            if (f->cast == film.cast)
                castEffect = ratingToCastEffect(rating);
        }

        return imdbScore + (directorEffect * 0.5) + (castEffect * 0.6);
    }

    double calculateScoreForNewUser(const Film& film) {
        int P = 0, A = 0, Z = 0;

        for (const auto& user : users) {
            for (size_t i = 0; i < user.watched.size(); ++i) {
                if (user.watched[i] == film.name) {
                    string r = user.ratings[i];
                    if (r == "Perfect") ++P;
                    else if (r == "Average") ++A;
                    else if (r == "Poor") ++Z;
                }
            }
        }

        return (P * 0.7) + (A * 0.5) + (Z * (-0.2));
    }

    void genreRecommendation(const string& username, const string& genre) {
        vector<pair<Film, double>> result;

        if (username.empty()) {
            for (const auto& film : films) {
                if (film.genre == genre) {
                    double score = calculateScoreForNewUser(film);
                    result.push_back({film, score});
                }
            }
        } else {
            User* user = findUser(username);
            if (!user) {
                cout << "User not found." << endl;
                return;
            }

            for (const auto& film : films) {
                if (film.genre != genre) continue;

                if (find(user->watched.begin(), user->watched.end(), film.name) != user->watched.end())
                    continue;

                double score = calculateScoreForExistingUser(film, *user);
                result.push_back({film, score});
            }
        }

        sort(result.begin(), result.end(), [](const pair<Film, double>& a, const pair<Film, double>& b) {
            if (fabs(a.second - b.second) > 1e-6)
                return a.second > b.second;
            if (fabs(a.first.imdb - b.first.imdb) > 1e-6)
                return a.first.imdb > b.first.imdb;
            return a.first.name < b.first.name;
        });

        for (int i = 0; i < min(3, (int)result.size()); ++i) {
            cout << i + 1 << ". " << result[i].first.name
                 << ": " << result[i].first.director
                 << " (" << result[i].first.imdb << ")" << endl;
        }
    }

    void castRecommendation(const string& username, const string& castName) {
        vector<Film> candidates;

        set<string> watched;
        string fav_genre;

        if (!username.empty()) {
            User* user = findUser(username);
            if (!user) {
                cout << "User not found.\n";
                return;
            }

            for (const auto& f : user->watched)
                watched.insert(f);

            map<string, int> genre_count;
            for (const auto& f : user->watched) {
                Film* film = findFilm(f);
                if (film)
                    genre_count[film->genre]++;
            }

            int maxCount = 0;
            for (const auto& [g, count] : genre_count)
                if (count > maxCount) {
                    fav_genre = g;
                    maxCount = count;
                }

            for (const auto& film : films) {
                if (film.genre == fav_genre &&
                    film.cast == castName &&
                    watched.find(film.name) == watched.end()) {
                    candidates.push_back(film);
                }
            }

        } else {
            for (const auto& film : films) {
                if (film.cast == castName)
                    candidates.push_back(film);
            }
        }

        sort(candidates.begin(), candidates.end(), [](const Film& a, const Film& b) {
            if (fabs(a.imdb - b.imdb) > 1e-6)
                return a.imdb > b.imdb;
            return a.name < b.name;
        });

        if (candidates.empty()) {
            cout << "No suitable movies were found.\n";
        } else {
            for (int i = 0; i < min(2, (int)candidates.size()); ++i) {
                cout << i + 1 << ". " << candidates[i].name
                     << ": " << candidates[i].director
                     << " (" << candidates[i].imdb << ")" << endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./ChiBebinam users.csv films.csv\n";
        return 1;
    }

    MovieRecommender recommender;

    if (!recommender.loadUsers(argv[1]) || !recommender.loadFilms(argv[2])) {
        cerr << "Failed to load data files.\n";
        return 1;
    }

    cout << "\n=== ChiBebinam Movie Recommender ===\n";
    string input;
    while (true) {
        cout << "\nCommand: ";
        getline(cin, input);
        if (input == "exit") break;

        vector<size_t> quotes;
        for (size_t i = 0; i < input.size(); ++i)
            if (input[i] == '"') quotes.push_back(i);
        cout << input.substr(0, 19) <<endl;
        if (input.substr(0, 20) == "genre_recommendation") {
            if (quotes.size() == 2) {
                string genre = input.substr(quotes[0] + 1, quotes[1] - quotes[0] - 1);
                recommender.genreRecommendation("", genre);
            } else if (quotes.size() == 4) {
                string user = input.substr(quotes[0] + 1, quotes[1] - quotes[0] - 1);
                string genre = input.substr(quotes[2] + 1, quotes[3] - quotes[2] - 1);
                recommender.genreRecommendation(user, genre);
            } else {
                cout << "Invalid command format.\n";
            }
        } else if (input.substr(0, 19) == "cast_recommandation") {
            if (quotes.size() == 2) {
                string cast = input.substr(quotes[0] + 1, quotes[1] - quotes[0] - 1);
                recommender.castRecommendation("", cast);
            } else if (quotes.size() == 4) {
                string user = input.substr(quotes[0] + 1, quotes[1] - quotes[0] - 1);
                string cast = input.substr(quotes[2] + 1, quotes[3] - quotes[2] - 1);
                recommender.castRecommendation(user, cast);
            } else {
                cout << "Invalid command format.\n";
            }
        } else {
            cout << "Commands:\n";
            cout << "genre_recommendation \"<Username>\" \"<Genre>\"\n";
            cout << "cast_recommandation \"<Username>\" \"<Cast>\"\n";
            cout << "exit\n";
        }
    }

    return 0;
}
