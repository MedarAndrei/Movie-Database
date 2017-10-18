

#ifndef _HOME_ANDREI_POLI_ANUL1_SEM2_SD_TEMA3_TEST_INCLUDE_IMDB_H_
#define _HOME_ANDREI_POLI_ANUL1_SEM2_SD_TEMA3_TEST_INCLUDE_IMDB_H_

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
// using namespace std;

#define NONE "none"

#define ADD_MOVIE "add_movie"
#define ADD_USER "add_user"
#define ADD_ACTOR "add_actor"
#define ADD_RATING "add_rating"
#define UPDATE_RATING "update_rating"
#define REMOVE_RATING "remove_rating"
// queries
#define GET_RATING "get_rating"
#define GET_LONGEST_CAREER_ACTOR "get_longest_career_actor"
#define GET_MOST_INFLUENTIAL_DIRECTOR "get_most_influential_director"
#define GET_BEST_YEAR_FOR_CATEGORY "get_best_year_for_category"
#define GET_2ND_DEGREE_COLLEAGUES "get_2nd_degree_colleagues"
#define GET_TOP_K_MOST_RECENT_MOVIES "get_top_k_most_recent_movies"
#define GET_TOP_K_ACTOR_PAIRS "get_top_k_actor_pairs"
#define GET_TOP_K_PARTNERS_FOR_ACTOR "get_top_k_partners_for_actor"
#define GET_TOP_K_MOST_POPULAR_MOVIES "get_top_k_most_popular_movies"
#define GET_AVG_RATING_IN_RANGE "get_avg_rating_in_range"

struct movie {
  std::string movie_name;
  std::string movie_id;
  int timestamp;
  int nr_ratings;
  int64_t sum_rating;
};

struct user {
  std::string user_name;
  std::string user_id;
  std::vector<std::pair<std::string, int>> user_ratings;
  // toate ratingurile, pereche: movie_id, valoare rating
};

struct actor {
  std::string actor_name;
  std::string actor_id;
  int first_movie, last_movie;
  std::vector<std::pair<std::string, int>> actor_colleagues;
};

struct actor_pair {
  std::string actor1_id;
  std::string actor2_id;
  int count;
};

struct director {
  std::string director_name;
  std::set<std::string> actor_id;
};

class IMDb {
 public:
  IMDb();
  ~IMDb();

  // metodele noastre
  void add_edge(const std::string &s1, const std::string &s2);
  bool has_edge(const std::string &s1, const std::string &s2);

  // metodele initiale

  void add_movie(std::string movie_name, std::string movie_id, int timestamp,
                 std::vector<std::string> categories, std::string director_name,
                 std::vector<std::string> actor_ids);

  void add_user(std::string user_id, std::string name);
  void add_actor(std::string actor_id, std::string name);

  void add_rating(std::string user_id, std::string movie_id, int rating);
  void update_rating(std::string user_id, std::string movie_id, int rating);
  void remove_rating(std::string user_id, std::string movie_id);

  // queries
  std::string get_rating(std::string movie_id);
  std::string get_longest_career_actor();
  std::string get_most_influential_director();
  std::string get_best_year_for_category(std::string category);
  std::string get_2nd_degree_colleagues(std::string actor_id);
  std::string get_top_k_most_recent_movies(int k);
  std::string get_top_k_actor_pairs(int k);
  std::string get_top_k_partners_for_actor(int k, std::string actor_id);
  std::string get_top_k_most_popular_movies(int k);
  std::string get_avg_rating_in_range(int start, int end);

  // input reader helper.
  std::string read_input_line(const std::string &line);

 private:
  // add any variables you need here.
  std::vector<movie> movies;
  std::unordered_map<std::string, int> movie_id_to_movies_idx;
  std::map<int, std::string>
      timestamp_to_movie_id;  // map sortat dupa timestamp

  std::vector<actor> actors;
  std::unordered_map<std::string, int> actor_id_to_actors_idx;
  std::string longest_career_actor_id;
  int longest_career_of_actor;

  std::vector<director> directors;
  int best_director_idx;
  unsigned int best_director_nr_actors;

  std::unordered_map<std::string, std::vector<std::string>>
      categories_to_movie_id;

  std::vector<user> users;
  std::unordered_map<std::string, int> user_id_to_users_idx;
};

#endif  //  _HOME_ANDREI_POLI_ANUL1_SEM2_SD_TEMA3_TEST_INCLUDE_IMDB_H_
