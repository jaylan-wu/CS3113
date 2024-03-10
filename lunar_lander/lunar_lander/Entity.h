class Entity
{
private:
    int* m_animation_right = NULL, // move to the right
        * m_animation_left = NULL, // move to the left
        * m_animation_up   = NULL, // move upwards
        * m_animation_down = NULL; // move downwards

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_model_matrix;
    glm::vec3 m_scale_size = glm::vec3(1.0f, 1.0f, 0.0f);

    int m_width  = 1,
        m_height = 1;
    
    float m_score = 0;
    std::string m_type;
    std::string m_collision = "";
    
public:
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT = 0,
        RIGHT = 1,
        UP    = 2,
        DOWN  = 3;

    // ————— ANIMATION ————— //
    int** m_walking = new int* [4]
        {
            m_animation_left,
            m_animation_right,
            m_animation_up,
            m_animation_down
        };

    int m_animation_frames = 0,
        m_animation_index = 0,
        m_animation_cols = 0,
        m_animation_rows = 0;

    int* m_animation_indices = NULL;
    float m_animation_time = 0.0f;

    GLuint    m_texture_id;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;

    void update(float delta_time, Entity* collidable_entities, int collidable_entity_count);
    void render(ShaderProgram* program);
    
    void move_left()  { m_acceleration.x = -2.0f;
                        m_score -= 0.25f;};
    void move_right() { m_acceleration.x = 2.0f;
                        m_score -= 0.25f;};
    void move_up()    { m_acceleration.y = 1.0f;  };
    void move_down()  { m_acceleration.y = -1.0f; };

    // ————— GETTERS ————— //
    glm::vec3   const get_position()     const { return m_position; };
    glm::vec3   const get_velocity()     const { return m_velocity; };
    glm::vec3   const get_acceleration() const { return m_acceleration; };
    glm::vec3   const get_movement()     const { return m_movement; };
    float       const get_speed()        const { return m_speed; };
    float       const get_score()        const { return m_score; };
    std::string const get_collision()    const { return m_collision; };
    std::string const get_type()         const { return m_type; };

    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
    void const set_acceleration(glm::vec3 new_position) { m_acceleration = new_position; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
    void const set_speed(float new_speed) { m_speed = new_speed; };
    void const set_size(glm::vec3 new_size) { m_scale_size = new_size;
                                              m_width     *= m_scale_size.x;
                                              m_height    *= m_scale_size.y;};
    void const set_score(float new_score) { m_score = new_score; };
    void const set_type(std::string new_type) { m_type = new_type; };
    void const set_collision(std::string new_collision) { m_collision = new_collision; };
};
