#pragma once
#include <Box2D\Box2D.h>
#include "misc/delta_accumulator.h"
#include "entity_system/processing_system.h"

#include "../components/physics_component.h"
#include "../components/transform_component.h"

#include <functional>

using namespace augs;
using namespace entity_system;

class physics_system : public processing_system_templated<components::physics, components::transform> {
	misc::delta_accumulator accumulator;

	void reset_states();
	void smooth_states();

	struct contact_listener : public b2ContactListener {
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		world* world_ptr;
	};

	contact_listener listener;
public:
	struct stepped_timer {
		physics_system* owner;
		int current_step;

		stepped_timer(physics_system*);
		void reset();
		float get_milliseconds() const;
		float extract_milliseconds();

		unsigned get_steps() const;
		unsigned extract_steps();
	};

	int all_steps;

	std::function<void(world&)> prestepping_routine;
	std::function<void(world&)> poststepping_routine;

	float timestep_multiplier;
	int enable_interpolation;
	int ray_casts_per_frame;

	bool enable_motors = false;
	void enable_listener(bool flag);

	b2World b2world;
	physics_system();

	/* returns the number of steps made */
	unsigned process_entities(world&);
	void process_steps(world&, unsigned);

	void add(entity_id) override;
	void remove(entity_id) override;
	void clear() override;

	void configure_stepping(float fps, int max_updates_per_step);
	double get_timestep_ms();

	struct raycast_output {
		vec2<> intersection, normal;
		bool hit;
		b2Fixture* what_fixture = nullptr;
		entity_id what_entity;

		raycast_output() : hit(false), what_fixture(nullptr) {}
	};

	std::vector<raycast_output> ray_cast_all_intersections(vec2<> p1_meters, vec2<> p2_meters, b2Filter filter, entity_id ignore_entity = entity_id());
	
	struct edge_edge_output {
		vec2<> intersection;
		bool hit;
	};

	edge_edge_output edge_edge_intersection(vec2<> p1_meters, vec2<> p2_meters, vec2<> edge_p1, vec2<> edge_p2);

	raycast_output ray_cast(vec2<> p1_meters, vec2<> p2_meters, b2Filter filter, entity_id ignore_entity = entity_id());
	raycast_output ray_cast_px(vec2<> p1, vec2<> p2, b2Filter filter, entity_id ignore_entity = entity_id());
	
	vec2<> push_away_from_walls(vec2<> position, float radius, int ray_amount, b2Filter filter, entity_id ignore_entity = entity_id());
	float get_closest_wall_intersection(vec2<> position, float radius, int ray_amount, b2Filter filter, entity_id ignore_entity = entity_id());

	struct query_output {
		struct queried_result {
			b2Body* body;
			b2Vec2 location;

			bool operator<(const queried_result& b) const {
				return body < b.body;
			}
			bool operator==(const queried_result& b) const {
				return body == b.body;
			}
		};

		std::vector<b2Body*> bodies;
		std::vector<queried_result> details;
	};

	query_output query_square(vec2<> p1_meters, float side_meters, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());
	query_output query_square_px(vec2<> p1, float side, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());
	query_output query_aabb(vec2<> p1_meters, vec2<> p2_meters, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());
	query_output query_aabb_px(vec2<> p1, vec2<> p2, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());

	query_output query_body(augs::entity_system::entity_id, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());

	query_output query_polygon(const std::vector<vec2<>>& vertices, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());
	query_output query_shape(b2Shape*, b2Filter* filter = nullptr, entity_id ignore_entity = entity_id());
private:
	/* callback structure used in QueryAABB function to get all shapes near-by */
	struct query_aabb_input : b2QueryCallback {
		entity_id ignore_entity;
		b2Filter* filter = nullptr;
		std::set<b2Body*> output;
		std::vector<b2Fixture*> out_fixtures;

		bool ReportFixture(b2Fixture* fixture) override;
	};

	struct raycast_input : public b2RayCastCallback {
		entity_id subject;
		b2Filter* subject_filter = nullptr;
		
		bool save_all = false;
		raycast_output output;
		std::vector<raycast_output> outputs;

		bool ShouldRaycast(b2Fixture* fixture);
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	};
};