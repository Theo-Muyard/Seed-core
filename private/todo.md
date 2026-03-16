# Seed Core - Todo List

> A day ?

<!-- # 📋 Seed Core - Roadmap & Todo List

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

- ✅ **Audit complet des noms publics**
  - [x] Analyser tous les symboles exposés dans `seed.h`
  - [x] Ajouter / vérifier les conventions de nommage (snake_case, préfixes)
  - [x] Documenter / modifier les incohérences trouvées
  - [x] Créer une table de migration

- ✅ **Normaliser le code**
  - [x] Prefixer les fonctions / emums de leur contexte (`manager_*`, `dispatcher_*`, etc...)
  - [x] Ajouter / verifier la norme des systemes (internal,  commandes, system)
  - [x] S'assurer que les structures ont le préfixe `t_`, les enums `e_`, les structure `s_`

- ✅ **Homogénéiser les conventions de paramètres**
  - [x] Les erreurs / status toujours en dernier paramètre?
  - [x] Les handles toujours en premier?
  - [x] Les outputs toujours pointeurs?
  - [x] Documenter la convention finale

- ✅ **Renommer et migrer progressivement**
  - [x] Lister les changements de noms (old → new)
  - [x] Migrer les tests
  - [x] Migrer les examples/docs
  - [x] Retirer les anciens noms

---

### 2️⃣ Système d'Erreurs Robuste

**Impact:** Debuggage facile, traçabilité, contrôle d'erreur

- ✅ **Concevoir la structure d'erreur**
  - [x] Créer un enum `t_ErrorCode` avec tous les codes d'erreurs possibles
  - [x] Ajouter des codes pour chaque système (Manager, Dispatcher, Writing, FileSystem, etc...)
  - [x] Ajouter des codes pour les erreurs communes (allocation, validation, etc)
  - [x] Exemple : `ERR_BUFFER_NOT_FOUND`, `ERR_INVALID_PAYLOAD`, etc

- ✅ **Mettre à jour toutes les fonctions**
  - [x] Manager : `manager_exec()`, `manager_init()`, etc
  - [x] Dispatcher : toutes les fonctions
  - [x] Writing system : toutes les fonctions
  - [x] Tests : vérifier les codes d'erreur

---

### 3️⃣ Validation des Payloads Généralisée

**Impact:** Sécurité, stabilité, prévention de crash

- ✅ **Créer un validateur générique**
  - [x] Définir un schéma de validation (structure descriptive)
---

### 4️⃣ Système de Gestion de Fichiers

**Impact:** Persistance, import/export, intégration système

- ✅ **Spécifier l'API de fichiers**
  - [x] Créer les headers `includes/systems/filesystem`
  - [x] Définir les opérations : open, close, create, delete, move, save, read, write


- ✅ **Intégration avec le Manager**
  - [x] Ajouter les commandes filesystem au dispatcher
  - [x] Ajouter les commandes au header + handle erreurs

- ✅ **Gestion des erreurs fichiers**
  - [x] Permissions insuffisantes
  - [x] Fichier verrouillé (par autre processus)
  - [x] Encodage invalide
  - [x] Chemin invalide

- ⚠️ **Bonus : Watch des fichiers**
  - [ ] Détecter si fichier modifié par processus externe
  - [ ] Proposer reload ou merge
  - [ ] Optionnel pour v0.2.5?

---

### 5️⃣ Amélioration Générale

- ✅ **Couverture de tests**
  - [x] Viser >80% de couverture
  - [x] Tests pour tous les codes erreur
  - [x] Tests limites et edge cases
  - [x] Intégration tests (multi-système)

- ✅ **Documentation**
  - [x] Mettre à jour README avec v0.2 changes
  - [x] Documenter les codes erreur
  - [x] Ajouter examples pour filesystem
  - [x] Créer migration guide depuis v0.1

---

## 🚀 v0.3 - Fonctionnalités Avancées

> Après v0.2 stable | Nouvelles capabilities

- ✅ **Encodage unicode (UTF-8)**
  - [x] Passer d'ASCII a UNICODE : writing system
  - [x] Line: passage de len -> size
  - [x] Conversion index -> emplacement de l'unicode

- 🟨 **Resync fs**
  - [ ] Detecter changement dans le dossier root
  - [ ] Mettre a jour le VFS en fonction

- ⬜ **Amelioration de la stabilite / performances**
  - [ ] Verifier la gestion de la memoire (aucuns leaks)
  - [ ] Erreurs correctement gerees, architecture clean et fonctions bien ecrites
  - [ ] Optimiser si possible (sans suroptimiser)

- ⬜ **Couverture de tests**
  - [ ] Refaire tout le systeme de tests
  - [ ] Tester les commandes uniquement: tests unitaires style `exemple XX` (relation test - documentation)

- ⬜ **Documentation**
  - [ ] Creer un dossier `documentation`
  - [ ] Ajouter un fichier de documentation par systeme (writing, filesystem, ...)
  - [ ] Documenter l'API globale dans un fichier a part
  
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
- [x] Préfixes à utiliser: `seed_`, `manager_`, `dispatcher_`, etc
- [x] Codes erreur: enum vs defines?
- [x] Structure retour d'erreur: struct vs out param?
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
*Next Review: Après completion de v0.2* -->
