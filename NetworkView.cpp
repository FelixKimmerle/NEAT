#include "NetworkView.hpp"
#include <algorithm>
#include <cmath>

namespace
{
const sf::Color outline_color(150, 150, 150);

// White for 0, saturated green (positive) or red (negative) from magnitude 1 on.
sf::Color signed_color(float value)
{
    auto fade = static_cast<std::uint8_t>(255.f - std::min(std::abs(value), 1.f) * 255.f);
    return value < 0.f ? sf::Color(255, fade, fade) : sf::Color(fade, 255, fade);
}
} // namespace

NetworkView::NetworkView(const sf::Font &font) : font_(font)
{
}

void NetworkView::set_genome(const Genome &genome)
{
    genome_ = genome;
    values_.clear();
    rebuild();
}

void NetworkView::set_values(const std::map<unsigned int, float> &values)
{
    values_ = values;
    rebuild();
}

void NetworkView::set_bounds(const sf::FloatRect &bounds)
{
    bounds_ = bounds;
    rebuild();
}

void NetworkView::set_node_size(float size)
{
    node_size_ = std::max(size, 5.f);
    rebuild();
}

void NetworkView::show_innovations(bool show)
{
    show_innovations_ = show;
    rebuild();
}

void NetworkView::rebuild()
{
    lines_.clear();
    label_boxes_.clear();
    labels_.clear();
    nodes_.clear();
    node_texts_.clear();
    if (!genome_)
    {
        return;
    }
    const Genome &genome = *genome_;

    // Layer = longest path from a node without incoming connections. Outputs always go last.
    std::map<unsigned int, unsigned int> layer;
    unsigned int last_layer = 1;
    for (unsigned int node : genome.topological_order())
    {
        unsigned int l = genome.is_hidden(node) ? 1 : 0;
        for (const Gen &gene : genome.genes())
        {
            if (gene.is_enabled() && gene.to() == node)
            {
                l = std::max(l, layer[gene.from()] + 1);
            }
        }
        layer[node] = l;
        if (!genome.is_output(node))
        {
            last_layer = std::max(last_layer, l + 1);
        }
    }
    std::map<unsigned int, std::vector<unsigned int>> columns;
    for (unsigned int node : genome.nodes())
    {
        columns[genome.is_output(node) ? last_layer : layer[node]].push_back(node);
    }

    const float radius = node_size_ / 1.75f;
    std::map<unsigned int, sf::Vector2f> positions;
    for (const auto &[column, nodes] : columns)
    {
        float x = bounds_.position.x + radius + (bounds_.size.x - 2.f * radius) * column / last_layer;
        for (std::size_t i = 0; i < nodes.size(); i++)
        {
            float y = bounds_.position.y + bounds_.size.y * (i + 1.f) / (nodes.size() + 1.f);
            positions[nodes[i]] = {x, y};
        }
    }

    for (const Gen &gene : genome.genes())
    {
        if (!gene.is_enabled() && !show_innovations_)
        {
            continue;
        }
        sf::Vector2f from = positions[gene.from()];
        sf::Vector2f to = positions[gene.to()];
        if (gene.is_enabled())
        {
            lines_.emplace_back(from, to, 1.f + std::min(std::abs(gene.weight()), 3.f), signed_color(gene.weight()), radius);
        }
        else
        {
            lines_.emplace_back(from, to, 1.f, outline_color, radius);
        }

        if (show_innovations_)
        {
            sf::Text &label = labels_.emplace_back(font_, std::to_string(gene.innovation()), static_cast<unsigned int>(node_size_ * 0.6f));
            label.setFillColor(sf::Color::Black);
            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(bounds.position + bounds.size / 2.f);
            label.setPosition((from + to) / 2.f);

            bounds = label.getGlobalBounds();
            sf::RectangleShape &box = label_boxes_.emplace_back(bounds.size + sf::Vector2f(4.f, 4.f));
            box.setPosition(bounds.position - sf::Vector2f(2.f, 2.f));
            box.setFillColor(sf::Color::White);
            box.setOutlineColor(outline_color);
            box.setOutlineThickness(1.f);
        }
    }

    for (const auto &[node, position] : positions)
    {
        sf::CircleShape &circle = nodes_.emplace_back(radius);
        circle.setOrigin({radius, radius});
        circle.setPosition(position);
        circle.setOutlineColor(node == genome.bias_node() ? sf::Color(80, 120, 255) : outline_color);
        circle.setOutlineThickness(2.f);
        auto value = values_.find(node);
        circle.setFillColor(value != values_.end() ? signed_color(value->second) : sf::Color::White);

        // Longer ids get a smaller font so they still fit into the circle.
        const std::string id = std::to_string(node);
        const float text_scale = id.size() <= 2 ? 0.7f : 1.4f / id.size();
        sf::Text &text = node_texts_.emplace_back(font_, id, static_cast<unsigned int>(node_size_ * text_scale));
        text.setFillColor(sf::Color::Black);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + bounds.size / 2.f);
        text.setPosition(position);
    }
}

void NetworkView::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (const auto &line : lines_)
    {
        target.draw(line, states);
    }
    for (const auto &box : label_boxes_)
    {
        target.draw(box, states);
    }
    for (const auto &label : labels_)
    {
        target.draw(label, states);
    }
    for (const auto &node : nodes_)
    {
        target.draw(node, states);
    }
    for (const auto &text : node_texts_)
    {
        target.draw(text, states);
    }
}
