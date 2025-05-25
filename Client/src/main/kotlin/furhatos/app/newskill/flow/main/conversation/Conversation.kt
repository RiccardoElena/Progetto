package furhatos.app.newskill.flow.main.conversation

import furhatos.app.newskill.model.PersonalityVector
import furhatos.app.newskill.model.utils.helpers.GesturesHelper
import furhatos.app.newskill.model.utils.helpers.WaitingSentenceHelper
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state
import furhatos.flow.kotlin.utterance

@Suppress("ktlint:standard:function-naming")
fun Conversation(furhatPersonality: PersonalityVector) =
    state {
        val gestures = GesturesHelper.gesturesListFromPersonalityVector(furhatPersonality)
        onEntry {
            furhat.ask {
                random {
                    gestures.forEach { +it }
                }
                // placeholder
                +"Bene, cominciamo allora. Raccontami qualcosa di te"
            }
        }

        onResponse {
            val response =
                call {
                    furhat.say(async = true) {
                        random {
                            WaitingSentenceHelper.waitingSentence(furhatPersonality).forEach { +it }
                        }
                    }
                    // Here we need to call backend
                } as String?
            val utterance =
                response?.let {
                    utterance {
                        it.split(".").forEach { sentence ->
                            random {
                                gestures.forEach { gesture -> +gesture }
                            }
                            +sentence
                        }
                    }
                } ?: utterance {
                    random {
                        gestures.forEach { gesture -> +gesture }
                    }
                    +"Scusa, puoi ripetere?"
                }
            furhat.ask(utterance)
        }
    }
