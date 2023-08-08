#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include "bits/stdc++.h"
using namespace std;
#define int long long

template <typename T> int chmin(T&x,const T &y)
{
	if(x>y) {
		x=y;
		return 1;
	}
	return 0;
}
template <typename T> int chmax(T &x,const T &y)
{
	if(x<y) {
		x=y;
		return 1;
	}
	return 0;
}

// all times must be unique OR if have same in time then we can choice which one should go first

pair<vector<array<int, 3>>, vector<array<int, 5>>> get_stupid(vector<array<int, 2>> a, int w, int h) { // 2 x 2
	if (w < 2 || h < 2) return {};
	vector<array<int, 5>> actions;
	int n = a.size();
	sort(a.begin(), a.end());
	vector<array<int, 3>> ans;
	int cnt = (h - 1) * (w - (w + 2) / 3);
	set<int> free;
	for (int i = 0; i < cnt; i++) {
		free.insert(i);
	}
	vector<array<int, 2>> pos(n, {-1, -1});
	vector<int> pos_idx(n, -1);
	vector<array<int, 3>> act;
	for (int i = 0; i < n; i++) {
		act.push_back(array<int, 3> {a[i][0], 1, i});
		act.push_back(array<int, 3> {a[i][1], 0, i});
	}
	sort(act.begin(), act.end());
	auto get_pos_id = [&](int id) -> array<int, 2> {
		int r = id / (w - (w + 2) / 3) + 1, c = id % (w - (w + 2) / 3);
		c += (c + 1) / 2;
		return {r, c};
	};

	for (int i = 0; i < n * 2; i++) {
		if (act[i][1] == 1) {
			if (free.size()) {
				int id = *free.lower_bound(0);
				free.erase(id);
				auto [r, c] = get_pos_id(id);
				pos[act[i][2]] = {r, c};
				pos_idx[act[i][2]] = id;
				ans.push_back({act[i][2], r, c});
				actions.push_back({act[i][2], r, c, act[i][0], 0});
			}
		} else {
			if (pos[act[i][2]][0] != -1) {
				free.insert(pos_idx[act[i][2]]);
				actions.push_back({act[i][2], pos[act[i][2]][0], pos[act[i][2]][1], act[i][0], 1});
			}
		}
	}
	return {ans, actions};
}

pair<vector<array<int, 3>>, vector<array<int, 5>>> get_q(vector<array<int, 2>> a, int w, int h) { // we need 2 extra cols for every row
	if (h > 1 && w < 3 || h < 1 || w < 1) return {};
	vector<array<int, 5>> actions;
	vector<array<int, 3>> ans;

	int n = a.size();
	vector<array<int, 3>> b;
	vector<int> which(n, -1);
	for (int i = 0; i < n; i++) {
		b.push_back({a[i][0], a[i][1], i});
	}
	sort(b.begin(), b.end());
	vector<int> lost_chel(n);
	for (int i = 0; i < n; i++) {
		lost_chel[i] = i;
	}
	int qlen = w;
	if (h > 1) qlen -= 2;
	auto get_mx_from = [&](vector<array<int, 3>> a) {
		sort(a.begin(), a.end());
		vector<int> res;
		int n = a.size();
		vector<vector<int>> dp(n + 1, vector<int>(qlen + 1));
		vector<vector<array<int, 2>>> parent(n + 1, vector<array<int, 2>>(qlen + 1));
		for (int i = 0; i < n; i++) {
			for (int len = 1; len <= qlen; len++) {
				dp[i + 1][len] = 1;
				for (int j = i - 1; j >= 0; j--) {
					if (a[i][0] >= a[j][1]) {
						if (chmax(dp[i + 1][len], dp[j + 1][qlen] + 1)) {
							parent[i + 1][len] = {j + 1, qlen};
						}
					}
					if (a[i][1] >= a[j][1]) {
						if (chmax(dp[i + 1][len], dp[j + 1][len - 1] + 1)) {
							parent[i + 1][len] = {j + 1, len - 1};
						}
					}
				}
			}
		}

		array<int, 2> current = {0, qlen};
		for (int i = 0; i < n; i++) {
			if (dp[current[0]][current[1]] < dp[i + 1][qlen]) {
				current[0] = i + 1;
			}
		}

		while (dp[current[0]][current[1]] > 0) {
			res.push_back(current[0] - 1);
			auto p = parent[current[0]][current[1]];
			current = p;
		}
		reverse(res.begin(), res.end());
		return res;
	};
	for (int i = 0; i < h; i++) {
		vector<int> res = get_mx_from(b);
		for (auto j : res) {
			which[b[j][2]] = i;
		}
		// del
		set<int> deleted (res.begin(), res.end());
		vector<array<int, 3>> temp;
		for (int j = 0; j < b.size(); j++) {
			if (!deleted.count(j)) temp.push_back(b[j]);
		}
		b = temp;
	}

	vector<deque<int>> queues(h);
	vector<deque<int>> q_places(h);
	vector<array<int, 4>> act;
	vector<array<int, 2>> pos(n, {-1, -1});

	for (int i = 0; i < n; i++) {
		act.push_back({a[i][0], a[i][1], 1, i});
		act.push_back({a[i][1], a[i][0], 0, i});
	}

	sort(act.begin(), act.end());
	for (int i = 0; i < 2 * n; i++) {

		if (act[i][2] == 1) { // comes			
			if (which[act[i][3]] != -1) {
				int place = w - 1;
				if (h > 1) place--;
				if (queues[which[act[i][3]]].size()) {
					place = q_places[which[act[i][3]]].back() - 1;	
				}

				queues[which[act[i][3]]].push_back(act[i][3]);
				q_places[which[act[i][3]]].push_back(place);

				int r = which[act[i][3]], c = place;
				pos[act[i][3]] = {r, c};
				ans.push_back({act[i][3], r, c});
				actions.push_back({act[i][3], pos[act[i][3]][0], pos[act[i][3]][1], act[i][0], 0});
			}
		} else {
			if (which[act[i][3]] != -1) {
				assert(queues[which[act[i][3]]].front() == act[i][3]);
				actions.push_back({act[i][3], pos[act[i][3]][0], pos[act[i][3]][1], act[i][0], 1});
				queues[which[act[i][3]]].pop_front();
				q_places[which[act[i][3]]].pop_front();
			}
		}
	}
	return {ans, actions};
}

pair<vector<array<int, 3>>, vector<array<int, 5>>> get_st(vector<array<int, 2>> a, int w, int h) {
	if (h < 2 || w < 1) return {};

	vector<array<int, 3>> ans;
	vector<array<int, 5>> actions;

	int n = a.size();
	vector<array<int, 3>> b;
	vector<int> which(n, -1);
	for (int i = 0; i < n; i++) {
		b.push_back({a[i][0], a[i][1], i});
	}
	sort(b.begin(), b.end());
	vector<int> lost_chel(n);
	for (int i = 0; i < n; i++) {
		lost_chel[i] = i;
	}

	int stlen = h - 1;
	auto get_mx_from = [&](vector<array<int, 3>> a) {
		sort(a.begin(), a.end());
		vector<int> res;
		int n = a.size();
		vector<vector<int>> dp(n + 1, vector<int>(stlen + 1));
		vector<vector<array<int, 2>>> parent(n + 1, vector<array<int, 2>>(stlen + 1));
		for (int i = 0; i < n; i++) {
			for (int len = 1; len <= stlen; len++) {
				dp[i + 1][len] = 1;
				for (int j = i - 1; j >= 0; j--) {
					if (a[i][0] >= a[j][1]) {
						if (chmax(dp[i + 1][len], dp[j + 1][len] + 1)) {
							parent[i + 1][len] = {j + 1, len};
						}
					}
					if (a[i][1] <= a[j][1]) {
						if (chmax(dp[i + 1][len], dp[j + 1][len - 1] + 1)) {
							parent[i + 1][len] = {j + 1, len - 1};
						}
					}
				}
			}
		}
		array<int, 2> current = {0, stlen};
		for (int i = 0; i < n; i++) {
			if (dp[current[0]][current[1]] < dp[i + 1][stlen]) {
				current[0] = i + 1;
			}
		}
		while (dp[current[0]][current[1]] > 0) {
			res.push_back(current[0] - 1);
			auto p = parent[current[0]][current[1]];
			current = p;
		}
		reverse(res.begin(), res.end());
		return res;
	};
	for (int i = 0; i < w; i++) {
		vector<int> res = get_mx_from(b);
		for (auto j : res) {
			which[b[j][2]] = i;
		}
		// del
		set<int> deleted (res.begin(), res.end());
		vector<array<int, 3>> temp;
		for (int j = 0; j < b.size(); j++) {
			if (!deleted.count(j)) temp.push_back(b[j]);
		}
		b = temp;
	}


	vector<deque<int>> stacks(w);
	vector<array<int, 4>> act;
	vector<array<int, 2>> pos(n, {-1, -1});

	for (int i = 0; i < n; i++) {
		act.push_back({a[i][0], -a[i][1], 1, i});
		act.push_back({a[i][1], -a[i][0], 0, -i});
	}
	sort(act.begin(), act.end());

	for (int i = 0; i < 2 * n; i++) {
		if (act[i][2] == 1) { // comes
			if (which[act[i][3]] != -1) {
				int place = h - 1 - (int) stacks[which[act[i][3]]].size();
				stacks[which[act[i][3]]].push_back(act[i][3]);
				int r = place, c = which[act[i][3]];
				ans.push_back({act[i][3], r, c});
				pos[act[i][3]] = {r, c};
				actions.push_back({act[i][3], r, c, act[i][0], 0});

			}
		} else {
			act[i][3] *= -1;
			if (which[act[i][3]] != -1) {
				assert(stacks[which[act[i][3]]].back() == act[i][3]); 
				stacks[which[act[i][3]]].pop_back();
				actions.push_back({act[i][3], pos[act[i][3]][0], pos[act[i][3]][1], act[i][0], 1});
			}
		}
	}
	return {ans, actions};
}

pair<vector<array<int, 3>>, vector<array<int, 5>>> get_max_possible(vector<array<int, 2>> a, int w, int h) {
	auto stupid = get_stupid(a, w, h);
	auto q = get_q(a, w, h);
	auto st = get_st(a, w, h);

	auto res = stupid;
	if (q.first.size() > res.first.size()) {
		res = q;
	}
	if (st.first.size() > res.first.size()) {
		res = st;
	}
	return res;

}

string solve(int w, int h, vector<array<int, 2>> a) {
	int n = a.size();
	assert(n <= 1000 && w <= 1000 && h <= 1000 && n * n * w * h <= 1000000);
	for (int i = 0; i < n; i++) {
		assert(a[i][1] > a[i][0]);
	}
	auto [result, actions] = get_max_possible(a, w, h);
	string res;
	res += to_string(result.size()) + "\n";
	for (int i = 0; i < result.size(); i++) {
		res += to_string(result[i][0]) + " " + to_string(result[i][1]) + " " + to_string(result[i][2]) + " \n";
	}
	res += to_string(actions.size()) + "\n";
	for (int i = 0; i < actions.size(); i++) {
		res += to_string(actions[i][0]) + " " + to_string(actions[i][1]) + \
			" " + to_string(actions[i][2]) + " " + to_string(actions[i][3]) + " " + to_string(actions[i][4]) + " \n";
	}
	return res;
}


signed main() {
	// int t = 1;
	// // cin >> t;
	// while (t--) {
	// 	solve();
	// }
}

PYBIND11_MODULE(parking_lib, m) {

    m.def("solve", &solve);

};
