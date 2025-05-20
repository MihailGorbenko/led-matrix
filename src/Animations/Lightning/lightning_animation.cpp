#include "lightning_animation.hpp"
#include <algorithm>

LightningAnimation::LightningAnimation()
    : Animation("lightning", "Молния"),
      interval(DEFAULT_LIGHTNING_INTERVAL),
      intervalSetting("interval", "Интервал молний", &interval, "lightning", DEFAULT_LIGHTNING_INTERVAL, 500, 10000, 100),
      width(0), height(0)
{
    registerSetting(&intervalSetting);
    loadConfig();
}

AnimationType LightningAnimation::getType() const { return AnimationType::Lightning; }

void LightningAnimation::render(LedMatrix& matrix, AudioAnalyzer*) {
    width = matrix.getWidth();
    height = matrix.getHeight();
    CRGB* leds = matrix.getLeds();
    fill_solid(leds, width * height, CRGB::Black);

    unsigned long now = millis();

    // Генерируем новую молнию с заданным интервалом
    if (now - lastLightning > interval) {
        lastLightning = now;

        LightningBolt bolt;
        int x = random(0, width);
        int y = 0;
        bolt.path.push_back({x, y});
        while (y < height - 1) {
            int dx = random(-2, 3); // -2..2 для более резких поворотов
            x = constrain(x + dx, 0, width - 1);
            y++;
            bolt.path.push_back({x, y});
            if (bolt.path.size() > height * 2) break;
            // Несколько веток
            if (random8() < 40 && y < height - 3) {
                LightningBolt branch = bolt;
                int bx = constrain(x + random(-2, 3), 0, width - 1);
                int by = y + 1;
                branch.path.push_back({bx, by});
                branch.maxAge = 3 + random8(2); // ветки исчезают быстрее
                bolts.push_back(branch);
            }
        }
        bolts.push_back(bolt);
    }

    // Рисуем и старим молнии
    for (auto it = bolts.begin(); it != bolts.end();) {
        uint8_t brightness = 255 - it->age * 40;
        for (auto& p : it->path) {
            int px = p.first;
            int py = p.second;
            if (px >= 0 && px < width && py >= 0 && py < height) {
                leds[matrix.XY(px, py)] += CHSV(160, 0, brightness);
                // Толщина (ореол)
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = px + dx, ny = py + dy;
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                            leds[matrix.XY(nx, ny)] += CHSV(160, 0, brightness / 8);
                    }
                }
            }
        }
        it->age++;
        if (it->age > it->maxAge)
            it = bolts.erase(it);
        else
            ++it;
    }

    matrix.update();
    delay(40);
}