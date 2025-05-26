package furhatos.app.newskill.data.remote

import furhatos.app.newskill.data.remote.dto.MessageHistory
import furhatos.app.newskill.model.PersonalityDisplacement
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.util.Language
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json

object Parser {
    fun toRequest(
        pv: PersonalityDisplacement,
        lang: Language,
        history: MessageHistory,
    ): String {
        val request =
            "Extroversion = ${pv.extroversion}, Agreeableness = ${pv.agreeableness}" +
                ", Conscientiousness = ${pv.conscientiousness}, Emotional stability = ${pv.emotionalStability}, " +
                "Openness to experiences ${pv.opennessToExperiences}|${lang.code}|"

        val jsonString = Json.encodeToString(history.contents).removeSurrounding("[", "]")

        if (DEBUG_MODE) {
            println(jsonString)
        }
        return request + jsonString
    }

    fun toMessage(response: String): List<String> = response.split("|")
}
