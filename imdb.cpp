

#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "include/imdb.h"

bool cmp1(const std::pair<std::string, int> &p1,
          const std::pair<std::string, int> &p2) {
  if (p1.second > p2.second) {
    return true;
  }
  if (p1.second == p2.second && p1.first < p2.first) {
    return true;
  }
  return false;
}

bool cmp2(const actor_pair &p1, const actor_pair &p2) {
  if (p1.count > p2.count)
    return true;
  if (p1.count == p2.count && p1.actor1_id < p2.actor1_id)
    return true;
  if (p1.count == p2.count && p1.actor1_id == p2.actor1_id &&
      p1.actor2_id < p2.actor2_id)
    return true;
  return false;
}

// fc care aproximeaza cu zecimalele alea
std::string frumi(double a) {
  a = a * 100;
  a = round(a);
  a = a / 100;
  std::string s = std::to_string(a);
  while (s[s.size() - 3] != '.')
    s.pop_back();
  return s;
}

IMDb::IMDb() {
  // initialize what you need here.
  best_director_idx = -1;
  best_director_nr_actors = 0;
  longest_career_of_actor = -1;
}

IMDb::~IMDb() {}

void IMDb::add_edge(const std::string &s1, const std::string &s2) {
  int idx_i = actor_id_to_actors_idx[s1];
  int found = 0;
  for (unsigned k = 0; k < actors[idx_i].actor_colleagues.size(); k++)
    if (actors[idx_i].actor_colleagues[k].first == s2) {
      found = 1;
      actors[idx_i].actor_colleagues[k].second++;
    }
  if (!found) {
    std::pair<std::string, int> temp_pair(s2, 1);
    actors[idx_i].actor_colleagues.push_back(temp_pair);
  }
}

bool IMDb::has_edge(const std::string &s1, const std::string &s2) {
  int idx_i = actor_id_to_actors_idx[s1];
  auto it = find_if(
      actors[idx_i].actor_colleagues.begin(),
      actors[idx_i].actor_colleagues.end(),
      [&](const std::pair<std::string, int> &A) { return A.first == s2; });
  return it != actors[idx_i].actor_colleagues.end();
}

void IMDb::add_movie(std::string movie_name, std::string movie_id,
                     int timestamp,
                     std::vector<std::string> categories,
                     std::string director_name,
                     std::vector<std::string> actor_ids) {
  movie temp;
  temp.movie_name = movie_name;
  temp.movie_id = movie_id;
  temp.timestamp = timestamp;
  temp.nr_ratings = 0;
  temp.sum_rating = 0;
  movie_id_to_movies_idx[movie_id] = movies.size();
  movies.push_back(temp);

  timestamp_to_movie_id[timestamp] = movie_id;
  for (auto &it : categories) {
    categories_to_movie_id[it].push_back(movie_id);
  }
  int found_director = 0;
  int cur_director_idx = -1;
  for (unsigned int i = 0; i < directors.size(); i++) {
    if (directors[i].director_name == director_name) {
      cur_director_idx = i;
      for (unsigned int j = 0; j < actor_ids.size(); j++) {
        directors[i].actor_id.insert(actor_ids[j]);
      }
      found_director = 1;
      break;
    }
  }
  if (!found_director) {
    director temp_director;
    temp_director.director_name = director_name;
    for (unsigned i = 0; i < actor_ids.size(); i++) {
      temp_director.actor_id.insert(actor_ids[i]);
    }
    cur_director_idx = directors.size();
    directors.push_back(temp_director);
  }
  if (directors[cur_director_idx].actor_id.size() > best_director_nr_actors) {
    best_director_idx = cur_director_idx;
    best_director_nr_actors = directors[cur_director_idx].actor_id.size();
  }
  if (directors[cur_director_idx].actor_id.size() == best_director_nr_actors) {
    if (directors[cur_director_idx].director_name <
        directors[best_director_idx].director_name) {
      best_director_idx = cur_director_idx;
    }
  }

  if (actor_ids.size() > 1) {
    for (unsigned int i = 0; i < actor_ids.size() - 1; i++) {
      for (unsigned int j = i + 1; j < actor_ids.size(); j++) {
        add_edge(actor_ids[i], actor_ids[j]);
        add_edge(actor_ids[j], actor_ids[i]);
      }
    }
  }
  for (unsigned int i = 0; i < actor_ids.size(); i++) {
    int cur_actor_idx = actor_id_to_actors_idx[actor_ids[i]];
    if (actors[cur_actor_idx].first_movie == -1) {
      actors[cur_actor_idx].first_movie = timestamp;
      actors[cur_actor_idx].last_movie = timestamp;
    }

    if (actors[cur_actor_idx].first_movie > timestamp) {
      actors[cur_actor_idx].first_movie = timestamp;
    }

    if (actors[cur_actor_idx].last_movie < timestamp) {
      actors[cur_actor_idx].last_movie = timestamp;
    }
    if (actors[cur_actor_idx].last_movie - actors[cur_actor_idx].first_movie >
        longest_career_of_actor) {
      longest_career_of_actor =
          actors[cur_actor_idx].last_movie - actors[cur_actor_idx].first_movie;
      longest_career_actor_id = actors[cur_actor_idx].actor_id;
    }
    if (actors[cur_actor_idx].last_movie - actors[cur_actor_idx].first_movie ==
        longest_career_of_actor) {
      if (actors[cur_actor_idx].actor_id < longest_career_actor_id) {
        longest_career_actor_id = actors[cur_actor_idx].actor_id;
      }
    }
  }
}

void IMDb::add_user(std::string user_id, std::string name) {
  user temp;
  temp.user_name = name;
  temp.user_id = user_id;
  user_id_to_users_idx[user_id] = users.size();
  users.push_back(temp);
}

void IMDb::add_actor(std::string actor_id, std::string name) {
  actor temp;
  temp.actor_name = name;
  temp.actor_id = actor_id;
  temp.first_movie = -1;
  temp.last_movie = -2;
  actor_id_to_actors_idx[actor_id] = actors.size();
  actors.push_back(temp);
}

void IMDb::add_rating(std::string user_id, std::string movie_id, int rating) {
  int idx = user_id_to_users_idx[user_id];
  std::pair<std::string, int> ratingpair(movie_id, rating);
  users[idx].user_ratings.push_back(ratingpair);
  int idx_movie = movie_id_to_movies_idx[movie_id];
  movies[idx_movie].sum_rating += rating;
  movies[idx_movie].nr_ratings++;
}

void IMDb::update_rating(std::string user_id, std::string movie_id,
                         int rating) {
  int idx = user_id_to_users_idx[user_id];
  int idx_movie = movie_id_to_movies_idx[movie_id];
  // caut in user ratings, perechea cu movie_id dat
  auto it = find_if(
      users[idx].user_ratings.begin(), users[idx].user_ratings.end(),
      [&](std::pair<std::string, int> &A) { return A.first == movie_id; });
  movies[idx_movie].sum_rating += rating - it->second;
  it->second = rating;
}

void IMDb::remove_rating(std::string user_id, std::string movie_id) {
  int idx = user_id_to_users_idx[user_id];
  int idx_movie = movie_id_to_movies_idx[movie_id];

  // caut in user ratings, perechea cu movie_id dat
  auto it = find_if(
      users[idx].user_ratings.begin(), users[idx].user_ratings.end(),
      [&](std::pair<std::string, int> &A) { return A.first == movie_id; });
  movies[idx_movie].sum_rating -= it->second;
  movies[idx_movie].nr_ratings--;
  users[idx].user_ratings.erase(it);
}

std::string IMDb::get_rating(std::string movie_id) {
  int idx = movie_id_to_movies_idx[movie_id];
  if (movies[idx].nr_ratings == 0)
    return "none";
  double q = (double)movies[idx].sum_rating / movies[idx].nr_ratings;
  return frumi(q);
}

std::string IMDb::get_longest_career_actor() {
  if (longest_career_of_actor != -1) {
    return longest_career_actor_id;
  }
  return "none";
}

std::string IMDb::get_most_influential_director() {
  return directors[best_director_idx].director_name;
}

std::string IMDb::get_best_year_for_category(std::string category) {
  std::map<int, int> year_to_nr_ratings;
  std::map<int, double> year_to_sum_ratings;
  int year;
  int timestamp;
  int idx;
  for (const auto &it : categories_to_movie_id[category]) {
    idx = movie_id_to_movies_idx[it];
    if (movies[idx].nr_ratings > 0) {
      timestamp = movies[idx].timestamp;
      time_t temp = timestamp;
      struct tm local_tm;
      tm *tm = localtime_r(&temp, &local_tm);
      year = tm->tm_year + 1900;
      year_to_nr_ratings[year]++;
      year_to_sum_ratings[year] +=
          (double)movies[idx].sum_rating / (double)movies[idx].nr_ratings;
    }
  }
  int max_year;
  double max_rating = -1;
  double r;
  if (year_to_nr_ratings.size() == 0) {
    return "none";
  }
  for (const auto &it : year_to_sum_ratings) {
    r = it.second / (double)year_to_nr_ratings[it.first];
    if (r > max_rating) {
      max_rating = r;
      max_year = it.first;
    }
  }
  return std::to_string(max_year);
}

std::string IMDb::get_2nd_degree_colleagues(std::string actor_id) {
  int idx_i = actor_id_to_actors_idx[actor_id], idx_j;
  std::string n1, n2;
  std::set<std::string> v;
  for (const auto &it1 : actors[idx_i].actor_colleagues) {
    n1 = it1.first;
    idx_j = actor_id_to_actors_idx[n1];
    for (const auto &it2 : actors[idx_j].actor_colleagues) {
      n2 = it2.first;
      if (actor_id != n2 && !has_edge(actor_id, n2))
        v.insert(n2);
    }
  }
  if (v.size() == 0)
    return "none";
  std::string rez = "";
  for (const auto &it : v)
    rez += it + " ";
  rez.pop_back();
  return rez;
}

std::string IMDb::get_top_k_most_recent_movies(int k) {
  if (timestamp_to_movie_id.size() == 0) {
    return "none";
  }
  unsigned int y = 0;
  std::string top_k_movies = "";
  for (auto it = timestamp_to_movie_id.rbegin();
       y < (unsigned int)k && y < timestamp_to_movie_id.size(); ++it, ++y) {
    top_k_movies += it->second + " ";
  }
  top_k_movies.pop_back();
  return top_k_movies;
}

std::string IMDb::get_top_k_actor_pairs(int k) {
  std::vector<actor_pair> v;
  actor_pair temp;
  std::string id1, id2;
  for (int i = 0; i < (int)actors.size(); i++) {
    id1 = actors[i].actor_id;
    for (const auto &it : actors[i].actor_colleagues) {
      id2 = it.first;
      if (id1 < id2) {
        temp.actor1_id = id1;
        temp.actor2_id = id2;
        temp.count = it.second;
        v.push_back(temp);
      }
    }
  }
  if (v.size() == 0)
    return "none";

  std::string rez = "";
  if (k > (int)v.size()) {
    std::sort(v.begin(), v.end(), cmp2);
    for (const auto &it : v) {
      rez += "(" + it.actor1_id + " " + it.actor2_id + " " +
             std::to_string(it.count) + ")" + " ";
    }
    rez.pop_back();
    return rez;
  }
  std::partial_sort(v.begin(), v.begin() + k, v.end(), cmp2);
  int y = 0;
  for (auto it = v.begin(); y < k; ++it, ++y) {
    rez += "(" + it->actor1_id + " " + it->actor2_id + " " +
           std::to_string(it->count) + ")" + " ";
  }
  rez.pop_back();
  return rez;
  return "";
}

std::string IMDb::get_top_k_partners_for_actor(int k, std::string actor_id) {
  int idx = actor_id_to_actors_idx[actor_id];
  if (actors[idx].actor_colleagues.size() == 0) {
    return "none";
  }
  std::string rez = "";
  std::vector<std::pair<std::string, int>> v;
  std::pair<std::string, int> P;
  if (k > (int)actors[idx].actor_colleagues.size()) {
    std::sort(actors[idx].actor_colleagues.begin(),
              actors[idx].actor_colleagues.end(), cmp1);
    for (const auto &it : actors[idx].actor_colleagues) {
      rez += it.first + " ";
    }
    rez.pop_back();
    return rez;
  }
  std::partial_sort(actors[idx].actor_colleagues.begin(),
                    actors[idx].actor_colleagues.begin() + k,
                    actors[idx].actor_colleagues.end(), cmp1);
  int y = 0;
  for (auto it = actors[idx].actor_colleagues.begin(); y < k; ++it, ++y) {
    rez += it->first + " ";
  }
  rez.pop_back();
  return rez;
}

std::string IMDb::get_top_k_most_popular_movies(int k) {
  if (movies.size() == 0) {
    return "none";
  }
  std::vector<std::pair<std::string, int>> v;
  std::pair<std::string, int> P;
  for (const auto &it : movies) {
    P.first = it.movie_id;
    P.second = it.nr_ratings;
    v.push_back(P);
  }
  std::string rez = "";
  if (k > (int)movies.size()) {
    std::sort(v.begin(), v.end(), cmp1);
    for (const auto &it : v) {
      rez += it.first + " ";
    }
    rez.pop_back();
    return rez;
  }
  std::partial_sort(v.begin(), v.begin() + k, v.end(), cmp1);
  int y = 0;
  for (auto it = v.begin(); y < k; ++it, ++y) {
    rez += it->first + " ";
  }
  rez.pop_back();
  return rez;
}

std::string IMDb::get_avg_rating_in_range(int start, int end) {
  auto a = timestamp_to_movie_id.lower_bound(start);
  double sum = 0;
  int nr = 0;
  int id;
  if (a != timestamp_to_movie_id.end()) {
    for (auto &it = a; it->first <= end && it != timestamp_to_movie_id.end();
         ++it) {
      id = movie_id_to_movies_idx[it->second];
      if (movies[id].nr_ratings) {
        sum += (double)movies[id].sum_rating / (double)movies[id].nr_ratings;
        nr++;
      }
    }
  }
  if (nr == 0)
    return "none";
  double t;
  t = (double)sum / (double)nr;
  return frumi(t);
}
