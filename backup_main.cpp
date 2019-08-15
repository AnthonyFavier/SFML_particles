#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
	ParticleSystem(unsigned int count, float lifetime) :
	m_particles(count),
	m_vertices(sf::Points, count),
	m_lifetime(sf::seconds(lifetime)),
	m_emitter(0.f, 0.f)
	{
	}
	
	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}
	
	void update(sf::Time elapsed)
	{
		for(std::size_t i = 0;
		i < m_particles.size();
		i++)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;
			
			// if the particle is dead, respawn it
			if(p.lifetime <= sf::Time::Zero)
				resetParticle(i);
			
			// update the position of the corresponding vertex
			p.velocity.y += 80 * elapsed.asSeconds();
			m_vertices[i].position += p.velocity * elapsed.asSeconds();
			
			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}
	
	void init(sf::RenderWindow& window)
	{
		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		setEmitter(window.mapPixelToCoords(mouse));
		
		for(std::size_t i=0; i<m_particles.size(); i++)
			resetParticle(i);
	}
	
	void debug()
	{
		std::cout << std::endl;
		for(std::size_t i=0; i<m_particles.size(); i++)
		{
			Particle& p = m_particles[i];
			std::cout << "vx" << p.velocity.x << std::endl;
			std::cout << "vy" << p.velocity.y << std::endl;
		}
	}
	
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();
		
		// our particles don't use a texture
		states.texture = NULL;
		
		// draw the vertex array
		target.draw(m_vertices, states);
	}
	
private:
	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifetime;
		float mass;
	};
	
	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % (2 * m_lifetime.asMilliseconds()/3)) + m_lifetime.asMilliseconds()/3);
		m_particles[index].mass = 1;
		
		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
	}
	
	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(800, 800), "Particles");
	
	// create the particle system
	ParticleSystem particles(2000, 4);
	particles.init(window);
	
	// create a clock to track the elapsed time
	sf::Clock clock;
	
	// run the main loop
	while(window.isOpen())
	{
		// handle events
		sf::Event event;
		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				
				case sf::Event::KeyPressed:
					switch(event.key.code)
					{
						case sf::Keyboard::Escape:
							window.close();
							break;
					}
					break;
			}
		}
		
		// make the particle system emitter follow the mouse
		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		particles.setEmitter(window.mapPixelToCoords(mouse));
		
		// update it
		sf::Time elapsed = clock.restart();
		particles.update(elapsed);
		
		// draw it
		window.clear();
		window.draw(particles);
		window.display();
	}
	
	return 0;
}

