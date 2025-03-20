#pragma once

#include "Filter.hpp"

class ADSR
{
    private:
        enum class Phase
        {
            Idle,
            Attack,
            Decay,
            Sustain,
            Release
        };

        float SampleRate;                   // Sample rate.
        float CurrentTime = 0.0f;           // The current time value.
        float TimeIncrement;                // A pre-calculated time increment.
        Phase CurrentPhase = Phase::Idle;   // The current envelope phase.
        float CurrentLevel = 0.0f;          // The current envelope level.
        float RemainingTriggerFadeTime;     // The trigger fade time remaining.
        SinglePole TriggerFadeFilter;       // The trigger fade filter.
        
    public:
        float AttackTime;                   // Attack time in seconds.
        float DecayTime;                    // Decay time in seconds.
        float SustainLevel;                 // Sustain level range 0 - 1.
        float ReleaseTime;                  // Release time in seconds.

        // Sets the sample rate and recalculates the time increment.
        void SetSampleRate(float sample_rate);

        // Starts the attack phase.
        void Trigger(bool enable_fade = true);

        // Starts the release phase.
        void Release();
        
        // Get the current envelope level.
        float GetLevel();

        // Creates an ADSR envelope with the given parameters.
        ADSR(float sample_rate, float attack_time, float decay_time, float sustain_level, float release_time);
};