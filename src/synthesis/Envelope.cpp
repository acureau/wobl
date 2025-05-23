#include "Envelope.hpp"

void ADSR::SetSampleRate(float sample_rate)
{
    this->SampleRate = sample_rate;
    this->TimeIncrement = 1.0f / sample_rate;
}

void ADSR::Trigger(bool enable_fade)
{
    this->CurrentTime = 0.0f;
    this->CurrentPhase = Phase::Attack;
    this->RemainingTriggerFadeTime = enable_fade ? 0.1f : 0.0f;
}

void ADSR::Release()
{
    this->CurrentTime = 0.0f;
    this->CurrentPhase = Phase::Release;
}

float ADSR::GetLevel()
{
    // Calculate level.
    switch (this->CurrentPhase)
    {
        case Phase::Idle:
            this->CurrentLevel = 0.0f;
            break;

        case Phase::Attack:
            this->CurrentLevel = this->CurrentTime / this->AttackTime;
            if (this->CurrentTime >= this->AttackTime)
            {
                this->CurrentTime = 0.0f;
                this->CurrentPhase = Phase::Decay;
            }
            break;

        case Phase::Decay:
            this->CurrentLevel = 1.0f - (this->CurrentTime / this->DecayTime * (1.0f - this->SustainLevel));
            if (this->CurrentTime >= this->DecayTime)
            {
                this->CurrentTime = 0.0f;
                this->CurrentPhase = Phase::Sustain;
            }
            break;

        case Phase::Sustain:
            this->CurrentLevel = this->SustainLevel;
            break;

        case Phase::Release:
            this->CurrentLevel = this->SustainLevel * (1.0f - this->CurrentTime / this->ReleaseTime);
            if (this->CurrentTime >= this->ReleaseTime)
            {
                this->CurrentTime = 0.0f;
                this->CurrentPhase = Phase::Idle;
            }
            break;
    }

    // Apply fade filter if necessary.
    float filtered_level = this->TriggerFadeFilter.Filter(this->CurrentLevel);
    if (this->RemainingTriggerFadeTime > 0.0f)
    {
        this->CurrentLevel = filtered_level;
        this->RemainingTriggerFadeTime -= this->TimeIncrement;
    }

    // Increment time and return calculated level.
    this->CurrentTime += this->TimeIncrement;
    return this->CurrentLevel;
}

ADSR::ADSR(float sample_rate, float attack_time, float decay_time, float sustain_level, float release_time):
    AttackTime(attack_time), 
    DecayTime(decay_time),
    SustainLevel(sustain_level),
    ReleaseTime(release_time),
    TriggerFadeFilter(FilterType::LowPass, 0.005f)
{
    this->SetSampleRate(sample_rate);
}