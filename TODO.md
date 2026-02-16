# 📋 Seed Core - Roadmap & Todo List

> Un système backend haute performance pour opérations textuelles. Cette liste documente l'évolution planifiée et les tâches à accomplir.

---

## 📊 Légende de Statut

| Symbole | Statut | Signification |
|---------|--------|---------------|
| ⬜ | **À faire** | Non commencé |
| 🟨 | **En cours** | En développement |
| ✅ | **Terminé** | Fini et testé |
| ⚠️ | **Bloqué** | En attente de dépendance |
| 🔄 | **Révision** | Fait mais à revoir |

---

## 🎯 v0.2 - Stabilisation & Robustesse Core

> **Priorité MAX** | Fondations solides pour les futures versions

### 1️⃣ Refactorisation des Noms d'API

**Impact:** Homogénéité, lisibilité, cohérence globale

- 🟨 **Audit complet des noms publics**
  - [ ] Analyser tous les symboles exposés dans `seed.h`
  - [ ] Ajouter / vérifier les conventions de nommage (snake_case, préfixes)
  - [ ] Documenter / modifier les incohérences trouvées
  - [ ] Créer une table de migration

- ⬜ **Normaliser le code**
  - [ ] Prefixer les fonctions / emums de leur contexte (`manager_*`, `dispatcher_*`, etc...)
  - [ ] Ajouter / verifier la norme des systemes (internal,  commandes, system)
  - [ ] S'assurer que les structures ont le préfixe `t_`, les enums `e_`, les structure `s_`

- ⬜ **Homogénéiser les conventions de paramètres**
  - [ ] Les erreurs / status toujours en dernier paramètre?
  - [ ] Les handles toujours en premier?
  - [ ] Les outputs toujours pointeurs?
  - [ ] Documenter la convention finale

- ⬜ **Renommer et migrer progressivement**
  - [ ] Lister les changements de noms (old → new)
  - [ ] Migrer les tests
  - [ ] Migrer les examples/docs
  - [ ] Retirer les anciens noms

---

### 2️⃣ Système d'Erreurs Robuste

**Impact:** Debuggage facile, traçabilité, contrôle d'erreur

- ⬜ **Concevoir la structure d'erreur**
  - [ ] Créer un enum `t_ErrorCode` avec tous les codes d'erreurs possibles
  - [ ] Ajouter des codes pour chaque système (Manager, Dispatcher, Writing, FileSystem, etc...)
  - [ ] Ajouter des codes pour les erreurs communes (allocation, validation, etc)
  - [ ] Exemple : `ERR_BUFFER_NOT_FOUND`, `ERR_INVALID_PAYLOAD`, etc

- ⬜ **Créer la structure de retour**
  - [ ] Définir `t_Result` ou `t_Status` (avec code + message optionnel)
  - [ ] Ou créer des fonctions qui remplissent une `t_Error` en output
  - [ ] Décider: retour booléen → retour structuré ou output param?

- ⬜ **Mettre à jour toutes les fonctions**
  - [ ] Manager : `manager_exec()`, `manager_init()`, etc
  - [ ] Dispatcher : toutes les fonctions
  - [ ] Writing system : toutes les fonctions
  - [ ] Tests : vérifier les codes d'erreur

---

### 3️⃣ Validation des Payloads Généralisée

**Impact:** Sécurité, stabilité, prévention de crash

- ⬜ **Créer un validateur générique**
  - [ ] Définir un schéma de validation (structure descriptive)
  - [ ] Implémenter `validate_payload()` générique
  - [ ] Supporter min/max de taille, types, formats
  - [ ] Support des champs obligatoires vs optionnels

- ⬜ **Schémas de validation par commande**
  - [ ] Créer un schéma pour chaque command (MAP ou table)
  - [ ] Exemple : `CMD_WRITING_CREATE_BUFFER` → schema avec champs attendus
  - [ ] Documenter les schémas dans la docstring de la commande

- ⬜ **Validation centralisée dans dispatcher**
  - [ ] Valider chaque payload avant d'appeler le handler
  - [ ] Retourner l'erreur de validation si invalide
  - [ ] Logger les validations échouées

- ⬜ **Cas limites & sécurité**
  - [ ] NULL payloads (ok? ou erreur?)
  - [ ] Buffers overflow (size > MAX_PAYLOAD_SIZE?)
  - [ ] Champs string : vérifier null-termination
  - [ ] Nested structures : validation récursive?

- ⬜ **Tests exhaustifs**
  - [ ] Valeur nulle pour chaque champ requis
  - [ ] Valeurs limites (min, max)
  - [ ] Strings malformées
  - [ ] Payloads trop volumineux
  - [ ] Payloads vides

---

### 4️⃣ Système de Gestion de Fichiers

**Impact:** Persistance, import/export, intégration système

- ⬜ **Spécifier l'API de fichiers**
  - [ ] Créer les headers `includes/systems/filesystem`
  - [ ] Définir les opérations : create, delete, open, save, close
  - [ ] Supporter les chemins absolus et relatifs
  - [ ] Supporter encodage UTF-8 minimum

- ⬜ **Opération : Ouvrir un fichier**
  - [ ] `filesystem_open(path, mode)` → file_handle
  - [ ] Charger dans un nouveau buffer
  - [ ] Détecter l'encodage automatiquement
  - [ ] Retourner erreur si fichier inexistant
  - [ ] Mapping fichier ↔ buffer_id pour tracking

- ⬜ **Opération : Sauvegarder un buffer**
  - [ ] `filesystem_save(buffer_id, path)` → erreur ou succès
  - [ ] Supporter overwrite/no-overwrite
  - [ ] Créer le répertoire parent si besoin
  - [ ] Atomic write (écrire dans temp, puis rename)
  - [ ] Préserver permissions/timestamps optionnels

- ⬜ **Opération : Fermer un fichier**
  - [ ] `filesystem_close(file_handle)`
  - [ ] Nettoyer les ressources
  - [ ] Optionnel: sauvegarder avant fermeture?
  - [ ] Optionnel: demander confirmation si modifié?

- ⬜ **Intégration avec le Manager**
  - [ ] Ajouter les commandes filesystem au dispatcher
  - [ ] Mapping persistent des fichiers ouverts
  - [ ] Gestion des buffers < → > fichiers

- ⬜ **Gestion des erreurs fichiers**
  - [ ] Permissions insuffisantes
  - [ ] Disque plein
  - [ ] Fichier verrouillé (par autre processus)
  - [ ] Encodage invalide
  - [ ] Chemin invalide

- ⬜ **Bonus : Watch des fichiers**
  - [ ] Détecter si fichier modifié par processus externe
  - [ ] Proposer reload ou merge
  - [ ] Optionnel pour v0.2.5?

---

### 5️⃣ Amélioration Générale

- ⬜ **Couverture de tests**
  - [ ] Viser >80% de couverture
  - [ ] Tests pour tous les codes erreur
  - [ ] Tests limites et edge cases
  - [ ] Intégration tests (multi-système)

- ⬜ **Documentation**
  - [ ] Mettre à jour README avec v0.2 changes
  - [ ] Documenter les codes erreur
  - [ ] Ajouter examples pour filesystem
  - [ ] Créer migration guide depuis v0.1

- ⬜ **Performance**
  - [ ] Profiler les opérations critiques
  - [ ] Optimiser allocations mémoire
  - [ ] Benchmark vs baseline v0.1

- ⬜ **Code quality**
  - [ ] Lancer les linters
  - [ ] Fixer les warnings
  - [ ] Revoir les conventions de code

---

---

## 🚀 v0.3 - Fonctionnalités Avancées

> Après v0.2 stable | Nouvelles capabilities

### Features à explorer

- ⬜ **Undo/Redo System**
  - [ ] Stack d'opérations
  - [ ] Replay d'opérations
  - [ ] Limite de profondeur (configurable)

- ⬜ **Encodage multi-format**
  - [ ] UTF-8, UTF-16, Latin-1, etc
  - [ ] Détection automatique + override manuel
  - [ ] Conversion transparent

- ⬜ **Selection & Range**
  - [ ] Notion de selection dans buffer
  - [ ] Operations sur ranges (copy, delete, format)
  - [ ] Multi-selection?

- ⬜ **Search & Replace**
  - [ ] Find in buffer
  - [ ] Replace with validation
  - [ ] Regex support (optionnel)
  - [ ] Find in all files (filesystem)

- ⬜ **Diff & Merge**
  - [ ] Comparer deux buffers
  - [ ] Générer diffs
  - [ ] Merge strategies

- ⬜ **Collaboration (RTC)**
  - [ ] Operational Transforms?
  - [ ] CRDT?
  - [ ] Multi-client support
  - [ ] Conflict resolution

---

## 🎨 v0.4 - Optimisations & Scalabilité

> Performance & large-scale usage

- ⬜ **Compression de buffers**
  - [ ] Compression RLE optionnelle
  - [ ] Compression gzip pour stockage
  - [ ] Impact sur performance

- ⬜ **Pagination de fichiers**
  - [ ] Loading lazy des sections de fichiers
  - [ ] Virtual scrolling friendly
  - [ ] Memory-mapped files

- ⬜ **Caching intelligent**
  - [ ] Cache des opérations fréquentes
  - [ ] Invalidation strategy
  - [ ] Benchmarks d'impact

- ⬜ **Async I/O**
  - [ ] Non-blocking file operations
  - [ ] Threading support?
  - [ ] Callback/Event system

---

## 💎 v0.5 - L'Aventure Complète

> Écosystème riche autour de Seed

- ⬜ **Plugin System**
  - [ ] API pour extensions
  - [ ] Hot-reload de plugins
  - [ ] Sandbox/security

- ⬜ **Bindings Language**
  - [ ] Python bindings
  - [ ] Node.js bindings
  - [ ] Rust bindings

- ⬜ **LSP Integration**
  - [ ] Language Server Protocol
  - [ ] Intégration IDE
  - [ ] Diagnostics

- ⬜ **CLI Tool**
  - [ ] `seed` command-line tool
  - [ ] Operations batch
  - [ ] Scripting support

---

---

## 📈 Stats & Métriques

```
Version Actuelle: v0.1
Statut Général: ✅ Stable (Basic)

Roadmap Coverage:
├─ v0.2: [████████████████░░] 80% planned
├─ v0.3: [██████░░░░░░░░░░░░] 30% planned
├─ v0.4: [████░░░░░░░░░░░░░░] 20% planned
└─ v0.5: [██░░░░░░░░░░░░░░░░] 10% planned
```

---

## 🔗 Dépendances entre Versions

```
v0.1 (Stable)
    ↓
v0.2 (Fondations: Erreurs, Validation, FS)
    ├─→ v0.3 (Avancé: Undo/Redo, Search, etc)
    │   ↓
    │   v0.4 (Perf: Compression, Async)
    │       ↓
    │       v0.5 (Écosystème: Plugins, Bindings)
    │
    └─→ v0.21 (Patch pour v0.2: Bugfixes)
        ↓
        v0.22 (Minor improvements)
```

---

## 📝 Notes de Développement

### Conventions Décidées
- [ ] Préfixes à utiliser: `seed_`, `manager_`, `dispatcher_`, etc
- [ ] Codes erreur: enum vs defines?
- [ ] Structure retour d'erreur: struct vs out param?
- [ ] Max payload size: À décider

### Considérations Architecturales
- [ ] Thread-safety requise pour v0.3+?
- [ ] Versioning de l'API (backward compat?)
- [ ] Stabilité ABI (binary compatibility)?

### Ressources & References
- [ ] Style guide C (POSIX? GNU? Custom?)
- [ ] Error code reference: Voir POSIX errno
- [ ] Filesystem best practices: TBD

---

## 🎯 Next Steps Immédiats (Post v0.1)

1. **Cette semaine:** Audit des noms d'API + déc conventions
2. **Semaine 2:** Implémenter système d'erreurs
3. **Semaine 3:** Validation payloads
4. **Semaine 4:** Système fichiers + tests intégration
5. **Semaine 5:** Polish & documentation v0.2

---

*Last Updated: 2026-01-29*  
*Next Review: Après completion de v0.2*
