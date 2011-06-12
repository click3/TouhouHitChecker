
#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class StageSceneSetting : boost::noncopyable {
public:
	class StageSceneSettingImpl : boost::noncopyable {
	public:
		typedef boost::tuple<std::wstring, unsigned int> SCENE_ID_PAIR;
		StageSceneSettingImpl(const boost::property_tree::wptree &tree) :
			stage_no(GetStageNo(tree)), latter_half_length(GetLatterHalfLength(tree)),
			latter_half_length_min(GetLatterHalfLengthMin(tree)), scene_id_list(GetSceneIdList(tree))
		{
			BOOST_ASSERT(stage_no > 0);
			BOOST_ASSERT(stage_no < 8);
			BOOST_ASSERT(latter_half_length_min <= latter_half_length);
			for(unsigned int i = 0; i < 4; i++) {
				for(unsigned int l = 0; l < 4; l++) {
					BOOST_ASSERT(!scene_id_list[i][l].empty());
					BOOST_ASSERT(scene_id_list[i][l][0].get<1>() == 0);
					int id = -1;
					BOOST_FOREACH(const SCENE_ID_PAIR &pair, scene_id_list[i][l]) {
						BOOST_ASSERT(!pair.get<0>().empty());
						BOOST_ASSERT(id < static_cast<int>(pair.get<1>()));
						id = static_cast<int>(pair.get<1>());
					}
				}
			}
		}
		unsigned int GetStageNo(void) const {
			return stage_no;
		}
		unsigned int GetLatterHalfLength(void) const {
			return latter_half_length;
		}
		unsigned int GetLatterHalfLengthMin(void) const {
			return latter_half_length_min;
		}
		unsigned int CalcSceneId(unsigned int scene_index, unsigned int play_mode, unsigned int scene_count) {
			BOOST_ASSERT(scene_index < 4);
			BOOST_ASSERT(play_mode < 4);
			struct FindSceneId {
				FindSceneId(unsigned int scene_count) : scene_count(scene_count) { }
				unsigned int scene_count;
				int operator()(int value, const SCENE_ID_PAIR &pair) const {
					if(scene_count >= pair.get<1>()) {
						return value + 1;
					}
					return value;
				}
			};
			const std::vector<SCENE_ID_PAIR> &id_list = scene_id_list[scene_index][play_mode];
			const int scene_id = std::accumulate(id_list.begin(), id_list.end(), -1, FindSceneId(scene_count));
			BOOST_ASSERT(scene_id >= 0);
			const unsigned int result = static_cast<unsigned int>(scene_id);
			if(scene_index > 0) {
				return result + CalcSceneId(scene_index - 1, play_mode, UINT_MAX) + 1;
			}
			return result;
		}

		unsigned int CalcSceneFrameCount(unsigned int scene_index, unsigned int th13_scene_frame_count, unsigned int mid_boss_frame_count) const {
			if(scene_index != 2) {
				return th13_scene_frame_count;
			}
			return CalcLatterHalfStartFrame(mid_boss_frame_count) + th13_scene_frame_count;
		}

		void Print(void) const {
			const wchar_t * const root_format = 
				L"{\n"
				L"	\"stage_no\":%d,\n"
				L"	\"latter_half_length\":%d,\n"
				L"	\"latter_half_length_min\":%d,\n"
				L"%s\n"
				L"}";
			const wchar_t * const scene_name_list[4] = {L"first_half", L"mid_boss", L"latter_half", L"boss"};
			const wchar_t * const mode_list[4] = {L"easy", L"normal", L"hard", L"lunatic"};
			wchar_t scene_text[4096] = L"";
			for(unsigned int i = 0; i < 4; i++) {
				for(unsigned int l = 0; l < 4; l++) {
					wchar_t id_list[1024] = L"";
					for(unsigned int j = 0; j < scene_id_list[i][l].size(); j++) {
						if(j != 0) {
							WSTRCATF(id_list, L", ");
						}
						WSTRCATF(id_list, L"{\"%s\":%d}", scene_id_list[i][l][j].get<0>().c_str(), scene_id_list[i][l][j].get<1>());
					}
					if(i + l != 0) {
						WSTRCATF(scene_text, L",\n");
					}
					WSTRCATF(scene_text, L"	\"%s_%s\":[%s]", scene_name_list[i], mode_list[l], id_list);
				}
			}
			::wprintf(root_format, stage_no, latter_half_length, latter_half_length_min, scene_text);
			BOOST_FOREACH(wchar_t wc, scene_id_list[0][0][0].get<0>()) {
				printf("%04x ", wc);
			}
			printf("\n");
			std::basic_string<wchar_t> ws(L"‘O”¼“¹’†1");
			BOOST_FOREACH(wchar_t wc, ws) {
				printf("%04x ", wc);
			}
		}
	protected:
		const unsigned int stage_no;
		const unsigned int latter_half_length;
		const unsigned int latter_half_length_min;
		const std::vector<std::vector<std::vector<SCENE_ID_PAIR> > > scene_id_list;
		
		static unsigned int GetStageNo(const boost::property_tree::wptree &tree) {
			boost::optional<unsigned int> result = tree.get_optional<unsigned int>(L"stage_no");
			if(!result) {
				// TODO
				::exit(1);
			}
			return result.get();
		}
		static unsigned int GetLatterHalfLength(const boost::property_tree::wptree &tree) {
			boost::optional<unsigned int> result = tree.get_optional<unsigned int>(L"latter_half_length");
			if(!result) {
				// TODO
				::exit(1);
			}
			return result.get();
		}
		static unsigned int GetLatterHalfLengthMin(const boost::property_tree::wptree &tree) {
			boost::optional<unsigned int> result = tree.get_optional<unsigned int>(L"latter_half_length_min");
			if(!result) {
				// TODO
				::exit(1);
			}
			return result.get();
		}
		static bool GetSceneIdListImplImpl(std::vector<SCENE_ID_PAIR> &result, const boost::property_tree::wptree &tree) {
			BOOST_FOREACH(const boost::property_tree::wptree::value_type &child, tree) {
				if(child.second.size() != 1 || !child.first.empty()) {
					return false;
				}
				const boost::property_tree::wptree::value_type &value = *child.second.begin();
				if(value.first.empty() || value.second.data().empty()) {
					return false;
				}
				const boost::optional<unsigned int> scene_id_opt = value.second.get_value_optional<unsigned int>();
				if(!scene_id_opt) {
					return false;
				}
				if(result.size() == 0 && scene_id_opt.get() != 0) {
					return false;
				}
				result.push_back(boost::make_tuple(value.first, scene_id_opt.get()));
			}
			return true;
		}
		static bool GetSceneIdListImpl(std::vector<std::vector<SCENE_ID_PAIR> > &result, const boost::property_tree::wptree &tree, const wchar_t *name) {
			BOOST_ASSERT(name != NULL);
			result.resize(4);
			{
				boost::optional<const boost::property_tree::wptree &> id_list_opt = tree.get_child_optional(name);
				if(id_list_opt) {
					const bool impl_impl_result = GetSceneIdListImplImpl(result[0], id_list_opt.get());
					if(!impl_impl_result) {
						return false;
					}
					result[1] = result[2] = result[3] = result[0];
					return true;
				}
			}
			const wchar_t * const mode_list[4] = {L"easy", L"normal", L"hard", L"lunatic"};
			for(unsigned int i = 0; i < 4; i++) {
				wchar_t mode_name[256];
				WSPRINTF(mode_name, L"%s_%s", name, mode_list[i]);
				boost::optional<const boost::property_tree::wptree &> id_list_opt = tree.get_child_optional(mode_name);
				if(!id_list_opt) {
					return false;
				}
				const bool impl_impl_result = GetSceneIdListImplImpl(result[i], id_list_opt.get());
				if(!impl_impl_result) {
					return false;
				}
			}
			return true;
		}
		static std::vector<std::vector<std::vector<SCENE_ID_PAIR> > > GetSceneIdList(const boost::property_tree::wptree &tree) {
			std::vector<std::vector<std::vector<SCENE_ID_PAIR> > > result(4);
			const wchar_t * const name_list[4] = {L"first_half", L"mid_boss", L"latter_half", L"boss"};
			for(unsigned int i = 0; i < 4; i++) {
				const bool impl_result = GetSceneIdListImpl(result[i], tree, name_list[i]);
				if(!impl_result) {
					// TODO
					::exit(1);
				}
			}
			return result;
		}

		unsigned int CalcLatterHalfStartFrame(unsigned int mid_boss_frame_count) const {
			const unsigned int latter_half_length = GetLatterHalfLength();
			const unsigned int latter_half_length_min = GetLatterHalfLengthMin();
			if(mid_boss_frame_count >= latter_half_length - latter_half_length_min) {
				return latter_half_length - latter_half_length_min;
			}
			return mid_boss_frame_count;
		}
	};
	bool Load(const boost::filesystem::path &json_path) {
		boost::property_tree::wptree json;
		try {
			read_json(json_path.string(), json, std::locale(std::locale::empty(), new boost::filesystem::detail::utf8_codecvt_facet));
		} catch(std::exception ex) {
			return false;
		}
		boost::optional<boost::property_tree::wptree &> stage_list_ptree = json.get_child_optional(L"stage_list");
		if(!stage_list_ptree) {
			return false;
		}
		BOOST_FOREACH(const boost::property_tree::wptree::value_type &child, stage_list_ptree.get()) {
			boost::shared_ptr<StageSceneSettingImpl> stage(new StageSceneSettingImpl(child.second));
			stage_list.push_back(stage);
		}
		return true;
	}
	bool IsLoaded(void) const {
		return !stage_list.empty();
	}
	bool Reset(void) {
		stage_list.clear();
		return true;
	}
	unsigned int CalcSceneId(unsigned int stage_number, unsigned int play_mode, unsigned int th13_scene_id, unsigned int th13_scene_count, unsigned int th13_scene_frame_count, unsigned int mid_boss_frame_count) const {
		boost::shared_ptr<StageSceneSettingImpl> stage = stage_list[stage_number - 1];
		const unsigned int scene_index = CalcSceneIndex(th13_scene_id, th13_scene_count);
		const unsigned int scene_frame_count = stage->CalcSceneFrameCount(scene_index, th13_scene_frame_count, mid_boss_frame_count);
		const unsigned int scene_count = CalcSceneCount(scene_index, th13_scene_count, scene_frame_count);
		return stage->CalcSceneId(scene_index, play_mode, scene_count);
	}
protected:
	std::vector<boost::shared_ptr<StageSceneSettingImpl> > stage_list;

	static unsigned int CalcSceneIndex(unsigned int th13_scene_id, unsigned int th13_scene_count) {
		unsigned int result;
		switch(th13_scene_id) {
			case 0:
				BOOST_ASSERT(th13_scene_count < 2);
				result = (th13_scene_count == 0 ? 0 : 2);
				break;
			case 2:
				result = 1;
				break;
			case 3:
				result = 3;
				break;
			default:
				BOOST_ASSERT(false);
				result = 0;
				break;
		}
		return result;
	}
	static unsigned int CalcSceneCount(unsigned int scene_index, unsigned int th13_scene_count, unsigned int scene_frame_count) {
		unsigned int result;
		switch(scene_index) {
			case 0:
			case 2:
				result = scene_frame_count;
				break;
			case 1:
			case 3:
				result = th13_scene_count;
				break;
			default:
				BOOST_ASSERT(false);
				result = 0;
				break;
		}
		return result;
	}
};
#pragma warning(pop)
#pragma pack(pop)
