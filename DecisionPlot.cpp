#include "DecisionPlot.hpp"
#include "NN.hpp"
#include <stdexcept>

namespace
{
sf::Color mix(sf::Color a, sf::Color b, float t)
{
    auto channel = [t](std::uint8_t x, std::uint8_t y) { return static_cast<std::uint8_t>(x + (y - x) * t); };
    return {channel(a.r, b.r), channel(a.g, b.g), channel(a.b, b.b)};
}

const sf::Color zero_color(45, 45, 80);
const sf::Color one_color(70, 190, 100);
} // namespace

DecisionPlot::DecisionPlot()
{
    if (!texture_.resize({resolution, resolution}))
    {
        throw std::runtime_error("DecisionPlot: could not create texture");
    }
    texture_.setSmooth(true);
    background_.setOutlineColor(sf::Color(150, 150, 150));
    background_.setOutlineThickness(1.f);
}

void DecisionPlot::set(const Task &task, const Genome &genome)
{
    samples_ = task.samples;
    input_min_ = task.input_min;
    input_max_ = task.input_max;
    selected_ = std::min(selected_, samples_.size() - 1);

    NN nn(genome);
    sf::Image image({resolution, resolution});
    for (unsigned int py = 0; py < resolution; py++)
    {
        for (unsigned int px = 0; px < resolution; px++)
        {
            float x = input_min_ + (input_max_ - input_min_) * (px + 0.5f) / resolution;
            float y = input_max_ - (input_max_ - input_min_) * (py + 0.5f) / resolution;
            image.setPixel({px, py}, mix(zero_color, one_color, nn.activate({x, y})[0]));
        }
    }
    texture_.update(image);

    outputs_.clear();
    for (const Sample &sample : samples_)
    {
        outputs_.push_back(nn.activate(sample.inputs)[0]);
    }
    rebuild();
}

void DecisionPlot::set_selected(std::size_t sample)
{
    selected_ = sample;
    rebuild();
}

void DecisionPlot::set_bounds(const sf::FloatRect &bounds)
{
    bounds_ = bounds;
    rebuild();
}

sf::Vector2f DecisionPlot::to_screen(const std::vector<float> &inputs) const
{
    // Samples on the edge of the input range are pulled in a bit so they stay fully visible.
    const float margin = 0.06f;
    auto normalize = [&](float value) { return margin + (1.f - 2.f * margin) * (value - input_min_) / (input_max_ - input_min_); };
    return {bounds_.position.x + bounds_.size.x * normalize(inputs[0]),
            bounds_.position.y + bounds_.size.y * (1.f - normalize(inputs[1]))};
}

void DecisionPlot::rebuild()
{
    background_.setPosition(bounds_.position);
    background_.setSize(bounds_.size);
    background_.setTexture(&texture_, true);

    points_.clear();
    const float radius = std::max(3.f, bounds_.size.x / 45.f);
    for (std::size_t i = 0; i < samples_.size() && i < outputs_.size(); i++)
    {
        sf::CircleShape &point = points_.emplace_back(radius);
        point.setOrigin({radius, radius});
        point.setPosition(to_screen(samples_[i].inputs));
        point.setFillColor(samples_[i].expected == 1.f ? sf::Color::White : sf::Color::Black);
        bool correct = is_correct(outputs_[i], samples_[i]);
        point.setOutlineColor(correct ? sf::Color(150, 150, 150) : sf::Color::Red);
        point.setOutlineThickness(correct ? 1.f : 2.5f);
    }
    if (selected_ < points_.size())
    {
        sf::CircleShape &ring = points_.emplace_back(radius * 2.f);
        ring.setOrigin({radius * 2.f, radius * 2.f});
        ring.setPosition(points_[selected_].getPosition());
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineColor(sf::Color::Yellow);
        ring.setOutlineThickness(2.f);
    }
}

void DecisionPlot::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(background_, states);
    for (const auto &point : points_)
    {
        target.draw(point, states);
    }
}
