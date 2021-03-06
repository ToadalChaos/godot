#include "baked_lightmap.h"
#include "io/resource_saver.h"
#include "os/dir_access.h"
#include "os/os.h"
#include "voxel_light_baker.h"

void BakedLightmapData::set_bounds(const AABB &p_bounds) {

	bounds = p_bounds;
	VS::get_singleton()->lightmap_capture_set_bounds(baked_light, p_bounds);
}

AABB BakedLightmapData::get_bounds() const {

	return bounds;
}

void BakedLightmapData::set_octree(const PoolVector<uint8_t> &p_octree) {

	VS::get_singleton()->lightmap_capture_set_octree(baked_light, p_octree);
}

PoolVector<uint8_t> BakedLightmapData::get_octree() const {

	return VS::get_singleton()->lightmap_capture_get_octree(baked_light);
}

void BakedLightmapData::set_cell_space_transform(const Transform &p_xform) {

	cell_space_xform = p_xform;
	VS::get_singleton()->lightmap_capture_set_octree_cell_transform(baked_light, p_xform);
}

Transform BakedLightmapData::get_cell_space_transform() const {
	return cell_space_xform;
}

void BakedLightmapData::set_cell_subdiv(int p_cell_subdiv) {
	cell_subdiv = p_cell_subdiv;
	VS::get_singleton()->lightmap_capture_set_octree_cell_subdiv(baked_light, p_cell_subdiv);
}

int BakedLightmapData::get_cell_subdiv() const {
	return cell_subdiv;
}

void BakedLightmapData::set_energy(float p_energy) {

	energy = p_energy;
	VS::get_singleton()->lightmap_capture_set_energy(baked_light, energy);
}

float BakedLightmapData::get_energy() const {

	return energy;
}

void BakedLightmapData::add_user(const NodePath &p_path, const Ref<Texture> &p_lightmap) {

	ERR_FAIL_COND(p_lightmap.is_null());
	User user;
	user.path = p_path;
	user.lightmap = p_lightmap;
	users.push_back(user);
}

int BakedLightmapData::get_user_count() const {

	return users.size();
}
NodePath BakedLightmapData::get_user_path(int p_user) const {

	ERR_FAIL_INDEX_V(p_user, users.size(), NodePath());
	return users[p_user].path;
}
Ref<Texture> BakedLightmapData::get_user_lightmap(int p_user) const {

	ERR_FAIL_INDEX_V(p_user, users.size(), Ref<Texture>());
	return users[p_user].lightmap;
}

void BakedLightmapData::clear_users() {
	users.clear();
}

void BakedLightmapData::_set_user_data(const Array &p_data) {

	ERR_FAIL_COND(p_data.size() & 1);

	for (int i = 0; i < p_data.size(); i += 2) {
		add_user(p_data[i], p_data[i + 1]);
	}
}

Array BakedLightmapData::_get_user_data() const {

	Array ret;
	for (int i = 0; i < users.size(); i++) {
		ret.push_back(users[i].path);
		ret.push_back(users[i].lightmap);
	}
	return ret;
}

RID BakedLightmapData::get_rid() const {
	return baked_light;
}
void BakedLightmapData::_bind_methods() {

	ClassDB::bind_method(D_METHOD("_set_user_data", "data"), &BakedLightmapData::_set_user_data);
	ClassDB::bind_method(D_METHOD("_get_user_data"), &BakedLightmapData::_get_user_data);

	ClassDB::bind_method(D_METHOD("set_bounds", "bounds"), &BakedLightmapData::set_bounds);
	ClassDB::bind_method(D_METHOD("get_bounds"), &BakedLightmapData::get_bounds);

	ClassDB::bind_method(D_METHOD("set_cell_space_transform", "xform"), &BakedLightmapData::set_cell_space_transform);
	ClassDB::bind_method(D_METHOD("get_cell_space_transform"), &BakedLightmapData::get_cell_space_transform);

	ClassDB::bind_method(D_METHOD("set_cell_subdiv", "cell_subdiv"), &BakedLightmapData::set_cell_subdiv);
	ClassDB::bind_method(D_METHOD("get_cell_subdiv"), &BakedLightmapData::get_cell_subdiv);

	ClassDB::bind_method(D_METHOD("set_octree", "octree"), &BakedLightmapData::set_octree);
	ClassDB::bind_method(D_METHOD("get_octree"), &BakedLightmapData::get_octree);

	ClassDB::bind_method(D_METHOD("set_energy", "energy"), &BakedLightmapData::set_energy);
	ClassDB::bind_method(D_METHOD("get_energy"), &BakedLightmapData::get_energy);

	ClassDB::bind_method(D_METHOD("add_user", "path", "lightmap"), &BakedLightmapData::add_user);
	ClassDB::bind_method(D_METHOD("get_user_count"), &BakedLightmapData::get_user_count);
	ClassDB::bind_method(D_METHOD("get_user_path", "user_idx"), &BakedLightmapData::get_user_path);
	ClassDB::bind_method(D_METHOD("get_user_lightmap", "user_idx"), &BakedLightmapData::get_user_lightmap);
	ClassDB::bind_method(D_METHOD("clear_users"), &BakedLightmapData::clear_users);

	ADD_PROPERTY(PropertyInfo(Variant::AABB, "bounds", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_bounds", "get_bounds");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_BYTE_ARRAY, "octree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_octree", "get_octree");
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM, "cell_space_transform", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_cell_space_transform", "get_cell_space_transform");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cell_subdiv", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_cell_subdiv", "get_cell_subdiv");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "energy", PROPERTY_HINT_RANGE, "0,16,0.01"), "set_energy", "get_energy");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "user_data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "_set_user_data", "_get_user_data");
}

BakedLightmapData::BakedLightmapData() {

	baked_light = VS::get_singleton()->lightmap_capture_create();
	energy = 1;
	cell_subdiv = 1;
}

BakedLightmapData::~BakedLightmapData() {

	VS::get_singleton()->free(baked_light);
}

///////////////////////////

BakedLightmap::BakeBeginFunc BakedLightmap::bake_begin_function = NULL;
BakedLightmap::BakeStepFunc BakedLightmap::bake_step_function = NULL;
BakedLightmap::BakeEndFunc BakedLightmap::bake_end_function = NULL;

void BakedLightmap::set_bake_subdiv(Subdiv p_subdiv) {
	bake_subdiv = p_subdiv;
}

BakedLightmap::Subdiv BakedLightmap::get_bake_subdiv() const {
	return bake_subdiv;
}

void BakedLightmap::set_capture_subdiv(Subdiv p_subdiv) {
	capture_subdiv = p_subdiv;
}

BakedLightmap::Subdiv BakedLightmap::get_capture_subdiv() const {
	return capture_subdiv;
}

void BakedLightmap::set_extents(const Vector3 &p_extents) {
	extents = p_extents;
	update_gizmo();
}

Vector3 BakedLightmap::get_extents() const {
	return extents;
}

void BakedLightmap::_find_meshes_and_lights(Node *p_at_node, List<PlotMesh> &plot_meshes, List<PlotLight> &plot_lights) {

	MeshInstance *mi = Object::cast_to<MeshInstance>(p_at_node);
	if (mi && mi->get_flag(GeometryInstance::FLAG_USE_BAKED_LIGHT) && mi->is_visible_in_tree()) {
		Ref<Mesh> mesh = mi->get_mesh();
		if (mesh.is_valid()) {

			bool all_have_uv2 = true;
			for (int i = 0; i < mesh->get_surface_count(); i++) {
				if (!(mesh->surface_get_format(i) & Mesh::ARRAY_FORMAT_TEX_UV2)) {
					all_have_uv2 = false;
					break;
				}
			}

			if (all_have_uv2 && mesh->get_lightmap_size_hint() != Size2()) {
				//READY TO BAKE! size hint could be computed if not found, actually..

				AABB aabb = mesh->get_aabb();

				Transform xf = get_global_transform().affine_inverse() * mi->get_global_transform();

				if (AABB(-extents, extents * 2).intersects(xf.xform(aabb))) {
					PlotMesh pm;
					pm.local_xform = xf;
					pm.mesh = mesh;
					pm.path = get_path_to(mi);
					for (int i = 0; i < mesh->get_surface_count(); i++) {
						pm.instance_materials.push_back(mi->get_surface_material(i));
					}
					pm.override_material = mi->get_material_override();
					plot_meshes.push_back(pm);
				}
			}
		}
	}

	Light *light = Object::cast_to<Light>(p_at_node);

	if (light && light->get_bake_mode() != Light::BAKE_DISABLED) {
		PlotLight pl;
		Transform xf = get_global_transform().affine_inverse() * light->get_global_transform();

		pl.local_xform = xf;
		pl.light = light;
		plot_lights.push_back(pl);
	}
	for (int i = 0; i < p_at_node->get_child_count(); i++) {

		Node *child = p_at_node->get_child(i);
		if (!child->get_owner())
			continue; //maybe a helper

		_find_meshes_and_lights(child, plot_meshes, plot_lights);
	}
}

void BakedLightmap::set_hdr(bool p_enable) {
	hdr = p_enable;
}

bool BakedLightmap::is_hdr() const {
	return hdr;
}

bool BakedLightmap::_bake_time(void *ud, float p_secs, float p_progress) {

	uint64_t time = OS::get_singleton()->get_ticks_usec();
	BakeTimeData *btd = (BakeTimeData *)ud;

	if (time - btd->last_step > 1000000) {

		int mins_left = p_secs / 60;
		int secs_left = Math::fmod(p_secs, 60.0f);
		int percent = p_progress * 100;
		bool abort = bake_step_function(btd->pass + percent, btd->text + " " + itos(percent) + "% (Time Left: " + itos(mins_left) + ":" + itos(secs_left) + "s)");
		btd->last_step = time;
		if (abort)
			return true;
	}

	return false;
}

BakedLightmap::BakeError BakedLightmap::bake(Node *p_from_node, bool p_create_visual_debug) {

	String save_path;

	if (image_path.begins_with("res://")) {
		save_path = image_path;
	} else {
		if (get_filename() != "") {
			save_path = get_filename().get_base_dir();
		} else if (get_owner() && get_owner()->get_filename() != "") {
			save_path = get_owner()->get_filename().get_base_dir();
		}

		if (save_path == "") {
			return BAKE_ERROR_NO_SAVE_PATH;
		}
		if (image_path != "") {
			save_path.plus_file(image_path);
		}
	}
	{
		//check for valid save path
		DirAccessRef d = DirAccess::open(save_path);
		if (!d) {
			ERR_PRINTS("Invalid Save Path: " + save_path);
			return BAKE_ERROR_NO_SAVE_PATH;
		}
	}

	Ref<BakedLightmapData> new_light_data;
	new_light_data.instance();

	static const int subdiv_value[SUBDIV_MAX] = { 8, 9, 10, 11, 12, 13 };

	VoxelLightBaker baker;

	baker.begin_bake(subdiv_value[bake_subdiv], AABB(-extents, extents * 2.0));

	List<PlotMesh> mesh_list;
	List<PlotLight> light_list;

	_find_meshes_and_lights(p_from_node ? p_from_node : get_parent(), mesh_list, light_list);

	if (bake_begin_function) {
		bake_begin_function(mesh_list.size() + light_list.size() + 1 + mesh_list.size() * 100);
	}

	int step = 0;

	int pmc = 0;

	for (List<PlotMesh>::Element *E = mesh_list.front(); E; E = E->next()) {

		if (bake_step_function) {
			bake_step_function(step++, RTR("Plotting Meshes: ") + " (" + itos(pmc + 1) + "/" + itos(mesh_list.size()) + ")");
		}

		pmc++;
		baker.plot_mesh(E->get().local_xform, E->get().mesh, E->get().instance_materials, E->get().override_material);
	}

	pmc = 0;
	baker.begin_bake_light(VoxelLightBaker::BakeQuality(bake_quality), VoxelLightBaker::BakeMode(bake_mode), propagation, energy);

	for (List<PlotLight>::Element *E = light_list.front(); E; E = E->next()) {

		if (bake_step_function) {
			bake_step_function(step++, RTR("Plotting Lights:") + " (" + itos(pmc + 1) + "/" + itos(light_list.size()) + ")");
		}

		pmc++;
		PlotLight pl = E->get();
		switch (pl.light->get_light_type()) {
			case VS::LIGHT_DIRECTIONAL: {
				baker.plot_light_directional(-pl.local_xform.basis.get_axis(2), pl.light->get_color(), pl.light->get_param(Light::PARAM_ENERGY), pl.light->get_param(Light::PARAM_INDIRECT_ENERGY), pl.light->get_bake_mode() == Light::BAKE_ALL);
			} break;
			case VS::LIGHT_OMNI: {
				baker.plot_light_omni(pl.local_xform.origin, pl.light->get_color(), pl.light->get_param(Light::PARAM_ENERGY), pl.light->get_param(Light::PARAM_INDIRECT_ENERGY), pl.light->get_param(Light::PARAM_RANGE), pl.light->get_param(Light::PARAM_ATTENUATION), pl.light->get_bake_mode() == Light::BAKE_ALL);
			} break;
			case VS::LIGHT_SPOT: {
				baker.plot_light_spot(pl.local_xform.origin, pl.local_xform.basis.get_axis(2), pl.light->get_color(), pl.light->get_param(Light::PARAM_ENERGY), pl.light->get_param(Light::PARAM_INDIRECT_ENERGY), pl.light->get_param(Light::PARAM_RANGE), pl.light->get_param(Light::PARAM_ATTENUATION), pl.light->get_param(Light::PARAM_SPOT_ANGLE), pl.light->get_param(Light::PARAM_SPOT_ATTENUATION), pl.light->get_bake_mode() == Light::BAKE_ALL);

			} break;
		}
	}
	/*if (bake_step_function) {
		bake_step_function(pmc++, RTR("Finishing Plot"));
	}*/

	baker.end_bake();

	Set<String> used_mesh_names;

	pmc = 0;
	for (List<PlotMesh>::Element *E = mesh_list.front(); E; E = E->next()) {

		String mesh_name = E->get().mesh->get_name();
		if (mesh_name == "" || mesh_name.find(":") != -1 || mesh_name.find("/") != -1) {
			mesh_name = "LightMap";
		}

		if (used_mesh_names.has(mesh_name)) {
			int idx = 2;
			String base = mesh_name;
			while (true) {
				mesh_name = base + itos(idx);
				if (!used_mesh_names.has(mesh_name))
					break;
				idx++;
			}
		}
		used_mesh_names.insert(mesh_name);

		pmc++;
		VoxelLightBaker::LightMapData lm;

		Error err;
		if (bake_step_function) {
			BakeTimeData btd;
			btd.text = RTR("Lighting Meshes: ") + mesh_name + " (" + itos(pmc) + "/" + itos(mesh_list.size()) + ")";
			btd.pass = step;
			btd.last_step = 0;
			err = baker.make_lightmap(E->get().local_xform, E->get().mesh, lm, _bake_time, &btd);
			if (err != OK) {
				bake_end_function();
				if (err == ERR_SKIP)
					return BAKE_ERROR_USER_ABORTED;
				return BAKE_ERROR_CANT_CREATE_IMAGE;
			}
			step += 100;
		} else {

			err = baker.make_lightmap(E->get().local_xform, E->get().mesh, lm);
		}

		if (err == OK) {

			Ref<Image> image;
			image.instance();

			uint32_t tex_flags = Texture::FLAGS_DEFAULT;
			if (hdr) {

				//just save a regular image
				PoolVector<uint8_t> data;
				int s = lm.light.size();
				data.resize(lm.light.size() * 2);
				{

					PoolVector<uint8_t>::Write w = data.write();
					PoolVector<float>::Read r = lm.light.read();
					uint16_t *hfw = (uint16_t *)w.ptr();
					for (int i = 0; i < s; i++) {
						hfw[i] = Math::make_half_float(r[i]);
					}
				}

				image->create(lm.width, lm.height, false, Image::FORMAT_RGBH, data);

			} else {

				//just save a regular image
				PoolVector<uint8_t> data;
				int s = lm.light.size();
				data.resize(lm.light.size());
				{

					PoolVector<uint8_t>::Write w = data.write();
					PoolVector<float>::Read r = lm.light.read();
					for (int i = 0; i < s; i += 3) {
						Color c(r[i + 0], r[i + 1], r[i + 2]);
						c = c.to_srgb();
						w[i + 0] = CLAMP(c.r * 255, 0, 255);
						w[i + 1] = CLAMP(c.g * 255, 0, 255);
						w[i + 2] = CLAMP(c.b * 255, 0, 255);
					}
				}

				image->create(lm.width, lm.height, false, Image::FORMAT_RGB8, data);

				//This texture is saved to SRGB for two reasons:
				// 1) first is so it looks better when doing the LINEAR->SRGB conversion (more accurate)
				// 2) So it can be used in the GLES2 backend, which does not support linkear workflow
				tex_flags |= Texture::FLAG_CONVERT_TO_LINEAR;
			}

			Ref<ImageTexture> tex;
			String image_path = save_path.plus_file(mesh_name + ".tex");
			bool set_path = true;
			if (ResourceCache::has(image_path)) {
				tex = Ref<Resource>((Resource *)ResourceCache::get(image_path));
				set_path = false;
			}

			if (!tex.is_valid()) {
				tex.instance();
			}

			tex->create_from_image(image, tex_flags);

			err = ResourceSaver::save(image_path, tex, ResourceSaver::FLAG_CHANGE_PATH);
			if (err != OK) {
				if (bake_end_function) {
					bake_end_function();
				}
				ERR_FAIL_COND_V(err != OK, BAKE_ERROR_CANT_CREATE_IMAGE);
			}

			if (set_path) {
				tex->set_path(image_path);
			}
			new_light_data->add_user(E->get().path, tex);
		}
	}

	int csubdiv = subdiv_value[capture_subdiv];
	AABB bounds = AABB(-extents, extents * 2);
	new_light_data->set_cell_subdiv(csubdiv);
	new_light_data->set_bounds(bounds);
	new_light_data->set_octree(baker.create_capture_octree(csubdiv));
	{

		Transform to_bounds;
		to_bounds.basis.scale(Vector3(bounds.get_longest_axis_size(), bounds.get_longest_axis_size(), bounds.get_longest_axis_size()));
		to_bounds.origin = bounds.position;

		Transform to_grid;
		to_grid.basis.scale(Vector3(1 << (csubdiv - 1), 1 << (csubdiv - 1), 1 << (csubdiv - 1)));

		Transform to_cell_space = to_grid * to_bounds.affine_inverse();
		new_light_data->set_cell_space_transform(to_cell_space);
	}

	if (bake_end_function) {
		bake_end_function();
	}

	//create the data for visual server

	if (p_create_visual_debug) {
		MultiMeshInstance *mmi = memnew(MultiMeshInstance);
		mmi->set_multimesh(baker.create_debug_multimesh(VoxelLightBaker::DEBUG_LIGHT));
		add_child(mmi);
#ifdef TOOLS_ENABLED
		if (get_tree()->get_edited_scene_root() == this) {
			mmi->set_owner(this);
		} else {
			mmi->set_owner(get_owner());
		}
#else
		mmi->set_owner(get_owner());
#endif
	}

	set_light_data(new_light_data);

	return BAKE_ERROR_OK;
}

void BakedLightmap::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {

		if (light_data.is_valid()) {
			_assign_lightmaps();
		}
		request_ready(); //will need ready again if re-enters tree
	}

	if (p_what == NOTIFICATION_EXIT_TREE) {

		if (light_data.is_valid()) {
			_clear_lightmaps();
		}
	}
}

void BakedLightmap::_assign_lightmaps() {

	ERR_FAIL_COND(!light_data.is_valid());

	for (int i = 0; i < light_data->get_user_count(); i++) {
		Node *node = get_node(light_data->get_user_path(i));
		VisualInstance *vi = Object::cast_to<VisualInstance>(node);
		ERR_CONTINUE(!vi);
		Ref<Texture> lightmap = light_data->get_user_lightmap(i);
		ERR_CONTINUE(!lightmap.is_valid());
		VS::get_singleton()->instance_set_use_lightmap(vi->get_instance(), get_instance(), lightmap->get_rid());
	}
}

void BakedLightmap::_clear_lightmaps() {
	ERR_FAIL_COND(!light_data.is_valid());
	for (int i = 0; i < light_data->get_user_count(); i++) {
		Node *node = get_node(light_data->get_user_path(i));
		VisualInstance *vi = Object::cast_to<VisualInstance>(node);
		ERR_CONTINUE(!vi);
		VS::get_singleton()->instance_set_use_lightmap(vi->get_instance(), RID(), RID());
	}
}

void BakedLightmap::set_light_data(const Ref<BakedLightmapData> &p_data) {

	if (light_data.is_valid()) {
		if (is_inside_tree()) {
			_clear_lightmaps();
		}
		set_base(RID());
	}
	light_data = p_data;

	if (light_data.is_valid()) {
		set_base(light_data->get_rid());
		if (is_inside_tree()) {
			_assign_lightmaps();
		}
	}
}

Ref<BakedLightmapData> BakedLightmap::get_light_data() const {

	return light_data;
}

void BakedLightmap::_debug_bake() {
	bake(get_parent(), true);
}

void BakedLightmap::set_propagation(float p_propagation) {
	propagation = p_propagation;
}

float BakedLightmap::get_propagation() const {

	return propagation;
}

void BakedLightmap::set_energy(float p_energy) {
	energy = p_energy;
}

float BakedLightmap::get_energy() const {

	return energy;
}

void BakedLightmap::set_bake_quality(BakeQuality p_quality) {
	bake_quality = p_quality;
}

BakedLightmap::BakeQuality BakedLightmap::get_bake_quality() const {
	return bake_quality;
}

void BakedLightmap::set_bake_mode(BakeMode p_mode) {
	bake_mode = p_mode;
}

BakedLightmap::BakeMode BakedLightmap::get_bake_mode() const {
	return bake_mode;
}

void BakedLightmap::set_image_path(const String &p_path) {
	image_path = p_path;
}

String BakedLightmap::get_image_path() const {
	return image_path;
}

AABB BakedLightmap::get_aabb() const {
	return AABB(-extents, extents * 2);
}
PoolVector<Face3> BakedLightmap::get_faces(uint32_t p_usage_flags) const {
	return PoolVector<Face3>();
}

void BakedLightmap::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_light_data", "data"), &BakedLightmap::set_light_data);
	ClassDB::bind_method(D_METHOD("get_light_data"), &BakedLightmap::get_light_data);

	ClassDB::bind_method(D_METHOD("set_bake_subdiv", "bake_subdiv"), &BakedLightmap::set_bake_subdiv);
	ClassDB::bind_method(D_METHOD("get_bake_subdiv"), &BakedLightmap::get_bake_subdiv);

	ClassDB::bind_method(D_METHOD("set_capture_subdiv", "capture_subdiv"), &BakedLightmap::set_capture_subdiv);
	ClassDB::bind_method(D_METHOD("get_capture_subdiv"), &BakedLightmap::get_capture_subdiv);

	ClassDB::bind_method(D_METHOD("set_bake_quality", "bake_quality"), &BakedLightmap::set_bake_quality);
	ClassDB::bind_method(D_METHOD("get_bake_quality"), &BakedLightmap::get_bake_quality);

	ClassDB::bind_method(D_METHOD("set_bake_mode", "bake_mode"), &BakedLightmap::set_bake_mode);
	ClassDB::bind_method(D_METHOD("get_bake_mode"), &BakedLightmap::get_bake_mode);

	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &BakedLightmap::set_extents);
	ClassDB::bind_method(D_METHOD("get_extents"), &BakedLightmap::get_extents);

	ClassDB::bind_method(D_METHOD("set_propagation", "propagation"), &BakedLightmap::set_propagation);
	ClassDB::bind_method(D_METHOD("get_propagation"), &BakedLightmap::get_propagation);

	ClassDB::bind_method(D_METHOD("set_energy", "energy"), &BakedLightmap::set_energy);
	ClassDB::bind_method(D_METHOD("get_energy"), &BakedLightmap::get_energy);

	ClassDB::bind_method(D_METHOD("set_hdr", "hdr"), &BakedLightmap::set_hdr);
	ClassDB::bind_method(D_METHOD("is_hdr"), &BakedLightmap::is_hdr);

	ClassDB::bind_method(D_METHOD("set_image_path", "image_path"), &BakedLightmap::set_image_path);
	ClassDB::bind_method(D_METHOD("get_image_path"), &BakedLightmap::get_image_path);

	ClassDB::bind_method(D_METHOD("bake", "from_node", "create_visual_debug"), &BakedLightmap::bake, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("debug_bake"), &BakedLightmap::_debug_bake);
	ClassDB::set_method_flags(get_class_static(), _scs_create("debug_bake"), METHOD_FLAGS_DEFAULT | METHOD_FLAG_EDITOR);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "bake_subdiv", PROPERTY_HINT_ENUM, "128,256,512,1024,2048,4096"), "set_bake_subdiv", "get_bake_subdiv");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "capture_subdiv", PROPERTY_HINT_ENUM, "128,256,512"), "set_capture_subdiv", "get_capture_subdiv");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bake_quality", PROPERTY_HINT_ENUM, "Low,Medium,High"), "set_bake_quality", "get_bake_quality");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bake_mode", PROPERTY_HINT_ENUM, "ConeTrace,RayTrace"), "set_bake_mode", "get_bake_mode");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "propagation", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_propagation", "get_propagation");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "energy", PROPERTY_HINT_RANGE, "0,32,0.01"), "set_energy", "get_energy");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hdr"), "set_hdr", "is_hdr");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "image_path", PROPERTY_HINT_DIR), "set_image_path", "get_image_path");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "extents"), "set_extents", "get_extents");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "light_data", PROPERTY_HINT_RESOURCE_TYPE, "BakedIndirectLightData"), "set_light_data", "get_light_data");

	BIND_ENUM_CONSTANT(SUBDIV_128);
	BIND_ENUM_CONSTANT(SUBDIV_256);
	BIND_ENUM_CONSTANT(SUBDIV_512);
	BIND_ENUM_CONSTANT(SUBDIV_1024);
	BIND_ENUM_CONSTANT(SUBDIV_2048);
	BIND_ENUM_CONSTANT(SUBDIV_4096);
	BIND_ENUM_CONSTANT(SUBDIV_MAX);

	BIND_ENUM_CONSTANT(BAKE_QUALITY_LOW);
	BIND_ENUM_CONSTANT(BAKE_QUALITY_MEDIUM);
	BIND_ENUM_CONSTANT(BAKE_QUALITY_HIGH);
	BIND_ENUM_CONSTANT(BAKE_MODE_CONE_TRACE);
	BIND_ENUM_CONSTANT(BAKE_MODE_RAY_TRACE);

	BIND_ENUM_CONSTANT(BAKE_ERROR_OK);
	BIND_ENUM_CONSTANT(BAKE_ERROR_NO_SAVE_PATH);
	BIND_ENUM_CONSTANT(BAKE_ERROR_NO_MESHES);
	BIND_ENUM_CONSTANT(BAKE_ERROR_CANT_CREATE_IMAGE);
	BIND_ENUM_CONSTANT(BAKE_ERROR_USER_ABORTED);
}

BakedLightmap::BakedLightmap() {

	extents = Vector3(10, 10, 10);
	bake_subdiv = SUBDIV_256;
	capture_subdiv = SUBDIV_128;
	bake_quality = BAKE_QUALITY_MEDIUM;
	bake_mode = BAKE_MODE_CONE_TRACE;
	energy = 1;
	propagation = 1;
	hdr = false;
	image_path = ".";
}
